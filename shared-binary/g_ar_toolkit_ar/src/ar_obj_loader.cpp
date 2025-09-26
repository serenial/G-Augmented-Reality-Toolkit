// this code is heavily cribbed from the tinyobjlib example/viewer.cc modified for LabVIEW
// https://github.com/tinyobjloader/tinyobjloader/blob/master/examples/viewer/viewer.cc

#include <algorithm>
#include <cstring>

#include <tiny_obj_loader.h>

#include "g_ar_toolkit/lv_interop/lv_error.hpp"
#include "g_ar_toolkit/lv_interop/lv_array_1d.hpp"
#include "g_ar_toolkit/lv_interop/lv_vec_types.hpp"

#include "g_ar_toolkit_ar_export.h"

#define TINYOBJLOADER_IMPLEMENTATION // define this in only *one* .cc
#define TINYOBJLOADER_USE_MAPBOX_EARCUT

using namespace g_ar_toolkit;
using namespace lv_interop;

// this is mostly a black box compared to the rest of the library so restrict
// these types to this translation unit - exported function is at the very bottom of this file
namespace
{

    struct objObject
    {
        int32_t vb_id; // vertex buffer id
        int32_t numTriangles;
        int32_t material_id;
        std::string name;
    };

    struct vec3
    {
        float v[3];
    };

    struct vec2
    {
        float v[2];
    };

#include "g_ar_toolkit/lv_interop/set_packing.hpp"
    struct LV_ObjPoint_t
    {
        vec3 vertex;
        vec3 normal;
        vec3 colour;
        vec2 text_coord;
        // this overloaded assignment operator allows copying from a flat std::vector
        // buffer of real_t/floats into this struct without making assumptions on the
        // byte packing
        LV_ObjPoint_t &operator=(std::vector<tinyobj::real_t>::iterator &it)
        {
            vertex.v[0] = *it++;
            vertex.v[1] = *it++;
            vertex.v[2] = *it++;

            normal.v[0] = *it++;
            normal.v[1] = *it++;
            normal.v[2] = *it++;

            colour.v[0] = *it++;
            colour.v[1] = *it++;
            colour.v[2] = *it++;

            text_coord.v[0] = *it++;
            text_coord.v[1] = *it;

            return *this;
        }
    };

    struct LV_ObjObject_t
    {
        LV_StringHandle_t name_handle;
        int32_t material_id;
    };

    struct LV_ObjMaterial_t
    {
        LV_StringHandle_t name_handle;
        float shininess;
        vec3 ambient, specular, emission, diffuse;
        LV_StringHandle_t diffuse_map_handle;
        LV_ObjMaterial_t &operator=(const tinyobj::material_t &material)
        {
            name_handle.copy_from(material.name);
            diffuse_map_handle.copy_from(material.diffuse_texname);
            shininess = material.shininess;

            ambient.v[0] = material.ambient[0];
            ambient.v[1] = material.ambient[1];
            ambient.v[2] = material.ambient[2];

            specular.v[0] = material.specular[0];
            specular.v[1] = material.specular[1];
            specular.v[2] = material.specular[2];

            emission.v[0] = material.emission[0];
            emission.v[1] = material.emission[1];
            emission.v[2] = material.emission[2];

            diffuse.v[0] = material.diffuse[0];
            diffuse.v[1] = material.diffuse[1];
            diffuse.v[2] = material.diffuse[2];

            return *this;
        }
    };

#include "g_ar_toolkit/lv_interop/reset_packing.hpp"

    void calcNormal(float N[3], float v0[3], float v1[3], float v2[3])
    {
        float v10[3];
        v10[0] = v1[0] - v0[0];
        v10[1] = v1[1] - v0[1];
        v10[2] = v1[2] - v0[2];

        float v20[3];
        v20[0] = v2[0] - v0[0];
        v20[1] = v2[1] - v0[1];
        v20[2] = v2[2] - v0[2];

        N[0] = v10[1] * v20[2] - v10[2] * v20[1];
        N[1] = v10[2] * v20[0] - v10[0] * v20[2];
        N[2] = v10[0] * v20[1] - v10[1] * v20[0];

        float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
        if (len2 > 0.0f)
        {
            float len = sqrtf(len2);

            N[0] /= len;
            N[1] /= len;
            N[2] /= len;
        }
    }

    void normalizeVector(vec3 &v)
    {
        float len2 = v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
        if (len2 > 0.0f)
        {
            float len = sqrtf(len2);

            v.v[0] /= len;
            v.v[1] /= len;
            v.v[2] /= len;
        }
    }

    // Check if `mesh_t` contains smoothing group id.
    bool hasSmoothingGroup(const tinyobj::shape_t &shape)
    {
        for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++)
        {
            if (shape.mesh.smoothing_group_ids[i] > 0)
            {
                return true;
            }
        }
        return false;
    }

    void computeSmoothingNormals(const tinyobj::attrib_t &attrib, const tinyobj::shape_t &shape,
                                 std::map<int, vec3> &smoothVertexNormals)
    {
        smoothVertexNormals.clear();
        std::map<int, vec3>::iterator iter;

        for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++)
        {
            // Get the three indexes of the face (all faces are triangular)
            tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
            tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
            tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

            // Get the three vertex indexes and coordinates
            int vi[3];     // indexes
            float v[3][3]; // coordinates

            for (int k = 0; k < 3; k++)
            {
                vi[0] = idx0.vertex_index;
                vi[1] = idx1.vertex_index;
                vi[2] = idx2.vertex_index;
                assert(vi[0] >= 0);
                assert(vi[1] >= 0);
                assert(vi[2] >= 0);

                v[0][k] = attrib.vertices[3 * vi[0] + k];
                v[1][k] = attrib.vertices[3 * vi[1] + k];
                v[2][k] = attrib.vertices[3 * vi[2] + k];
            }

            // Compute the normal of the face
            float normal[3];
            calcNormal(normal, v[0], v[1], v[2]);

            // Add the normal to the three vertexes
            for (size_t i = 0; i < 3; ++i)
            {
                iter = smoothVertexNormals.find(vi[i]);
                if (iter != smoothVertexNormals.end())
                {
                    // add
                    iter->second.v[0] += normal[0];
                    iter->second.v[1] += normal[1];
                    iter->second.v[2] += normal[2];
                }
                else
                {
                    smoothVertexNormals[vi[i]].v[0] = normal[0];
                    smoothVertexNormals[vi[i]].v[1] = normal[1];
                    smoothVertexNormals[vi[i]].v[2] = normal[2];
                }
            }

        } // f

        // Normalize the normals, that is, make them unit vectors
        for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end();
             iter++)
        {
            normalizeVector(iter->second);
        }

    } // computeSmoothingNormals

    void computeAllSmoothingNormals(tinyobj::attrib_t &attrib,
                                    std::vector<tinyobj::shape_t> &shapes)
    {
        vec3 p[3];
        for (size_t s = 0, slen = shapes.size(); s < slen; ++s)
        {
            const tinyobj::shape_t &shape(shapes[s]);
            size_t facecount = shape.mesh.num_face_vertices.size();
            assert(shape.mesh.smoothing_group_ids.size());

            for (size_t f = 0, flen = facecount; f < flen; ++f)
            {
                for (unsigned int v = 0; v < 3; ++v)
                {
                    tinyobj::index_t idx = shape.mesh.indices[3 * f + v];
                    assert(idx.vertex_index != -1);
                    p[v].v[0] = attrib.vertices[3 * idx.vertex_index];
                    p[v].v[1] = attrib.vertices[3 * idx.vertex_index + 1];
                    p[v].v[2] = attrib.vertices[3 * idx.vertex_index + 2];
                }

                // cross(p[1] - p[0], p[2] - p[0])
                float nx = (p[1].v[1] - p[0].v[1]) * (p[2].v[2] - p[0].v[2]) -
                           (p[1].v[2] - p[0].v[2]) * (p[2].v[1] - p[0].v[1]);
                float ny = (p[1].v[2] - p[0].v[2]) * (p[2].v[0] - p[0].v[0]) -
                           (p[1].v[0] - p[0].v[0]) * (p[2].v[2] - p[0].v[2]);
                float nz = (p[1].v[0] - p[0].v[0]) * (p[2].v[1] - p[0].v[1]) -
                           (p[1].v[1] - p[0].v[1]) * (p[2].v[0] - p[0].v[0]);

                // Don't normalize here.
                for (unsigned int v = 0; v < 3; ++v)
                {
                    tinyobj::index_t idx = shape.mesh.indices[3 * f + v];
                    attrib.normals[3 * idx.normal_index] += nx;
                    attrib.normals[3 * idx.normal_index + 1] += ny;
                    attrib.normals[3 * idx.normal_index + 2] += nz;
                }
            }
        }

        assert(attrib.normals.size() % 3 == 0);
        for (size_t i = 0, nlen = attrib.normals.size() / 3; i < nlen; ++i)
        {
            tinyobj::real_t &nx = attrib.normals[3 * i];
            tinyobj::real_t &ny = attrib.normals[3 * i + 1];
            tinyobj::real_t &nz = attrib.normals[3 * i + 2];
            tinyobj::real_t len = sqrtf(nx * nx + ny * ny + nz * nz);
            tinyobj::real_t scale = len == 0 ? 0 : 1 / len;
            nx *= scale;
            ny *= scale;
            nz *= scale;
        }
    }

    void compute_smoothing_shape(const tinyobj::attrib_t &inattrib, const tinyobj::shape_t &inshape,
                               std::vector<std::pair<unsigned int, unsigned int>> &sortedids,
                               unsigned int idbegin, unsigned int idend,
                               std::vector<tinyobj::shape_t> &outshapes,
                               tinyobj::attrib_t &outattrib)
    {
        unsigned int sgroupid = sortedids[idbegin].first;
        bool hasmaterials = inshape.mesh.material_ids.size();
        // Make a new shape from the set of faces in the range [idbegin, idend).
        outshapes.emplace_back();
        tinyobj::shape_t &outshape = outshapes.back();
        outshape.name = inshape.name;
        // Skip lines and points.

        std::unordered_map<unsigned int, unsigned int> remap;
        for (unsigned int id = idbegin; id < idend; ++id)
        {
            unsigned int face = sortedids[id].second;

            outshape.mesh.num_face_vertices.push_back(3); // always triangles
            if (hasmaterials)
                outshape.mesh.material_ids.push_back(inshape.mesh.material_ids[face]);
            outshape.mesh.smoothing_group_ids.push_back(sgroupid);
            // Skip tags.

            for (unsigned int v = 0; v < 3; ++v)
            {
                tinyobj::index_t inidx = inshape.mesh.indices[3 * face + v], outidx;
                assert(inidx.vertex_index != -1);
                auto iter = remap.find(inidx.vertex_index);
                // Smooth group 0 disables smoothing so no shared vertices in that case.
                if (sgroupid && iter != remap.end())
                {
                    outidx.vertex_index = (*iter).second;
                    outidx.normal_index = outidx.vertex_index;
                    outidx.texcoord_index = (inidx.texcoord_index == -1) ? -1 : outidx.vertex_index;
                }
                else
                {
                    assert(outattrib.vertices.size() % 3 == 0);
                    unsigned int offset = static_cast<unsigned int>(outattrib.vertices.size() / 3);
                    outidx.vertex_index = outidx.normal_index = offset;
                    outidx.texcoord_index = (inidx.texcoord_index == -1) ? -1 : offset;
                    outattrib.vertices.push_back(inattrib.vertices[3 * inidx.vertex_index]);
                    outattrib.vertices.push_back(inattrib.vertices[3 * inidx.vertex_index + 1]);
                    outattrib.vertices.push_back(inattrib.vertices[3 * inidx.vertex_index + 2]);
                    outattrib.normals.push_back(0.0f);
                    outattrib.normals.push_back(0.0f);
                    outattrib.normals.push_back(0.0f);
                    if (inidx.texcoord_index != -1)
                    {
                        outattrib.texcoords.push_back(inattrib.texcoords[2 * inidx.texcoord_index]);
                        outattrib.texcoords.push_back(inattrib.texcoords[2 * inidx.texcoord_index + 1]);
                    }
                    remap[inidx.vertex_index] = offset;
                }
                outshape.mesh.indices.push_back(outidx);
            }
        }
    }

    void computeSmoothingShapes(const tinyobj::attrib_t &inattrib,
                                const std::vector<tinyobj::shape_t> &inshapes,
                                std::vector<tinyobj::shape_t> &outshapes,
                                tinyobj::attrib_t &outattrib)
    {
        for (size_t s = 0, slen = inshapes.size(); s < slen; ++s)
        {
            const tinyobj::shape_t &inshape = inshapes[s];

            unsigned int numfaces = static_cast<unsigned int>(inshape.mesh.smoothing_group_ids.size());
            assert(numfaces);
            std::vector<std::pair<unsigned int, unsigned int>> sortedids(numfaces);
            for (unsigned int i = 0; i < numfaces; ++i)
                sortedids[i] = std::make_pair(inshape.mesh.smoothing_group_ids[i], i);
            sort(sortedids.begin(), sortedids.end());

            unsigned int activeid = sortedids[0].first;
            unsigned int id = activeid, idbegin = 0, idend = 0;
            // Faces are now bundled by smoothing group id, create shapes from these.
            while (idbegin < numfaces)
            {
                while (activeid == id && ++idend < numfaces)
                    id = sortedids[idend].first;
                compute_smoothing_shape(inattrib, inshape, sortedids, idbegin, idend,
                                      outshapes, outattrib);
                activeid = id;
                idbegin = idend;
            }
        }
    }

    bool load_obj_and_convert(float bmin[3], float bmax[3],
                           std::vector<objObject> &drawObjects,
                           std::vector<tinyobj::material_t> &materials,
                           const std::string &inputFilePath,
                           const std::string &searchPath,
                           const bool forceRegenNormals,
                           std::vector<std::vector<tinyobj::real_t>> &shapeVerticies)
    {

        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = searchPath;
        reader_config.triangulate = true;

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(inputFilePath, reader_config))
        {
            if (!reader.Error().empty())
            {
                throw std::logic_error(reader.Error());
            }
        }

        if (!reader.Warning().empty())
        {
            throw std::logic_error(reader.Warning());
        }

        auto &inattrib = reader.GetAttrib();
        auto &inshapes = reader.GetShapes();
        materials = reader.GetMaterials();

        // Append `default` material
        materials.push_back(tinyobj::material_t());

        bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
        bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();

        bool regen_all_normals = forceRegenNormals || inattrib.normals.size() == 0;
        tinyobj::attrib_t outattrib;
        std::vector<tinyobj::shape_t> outshapes;
        if (regen_all_normals)
        {
            computeSmoothingShapes(inattrib, inshapes, outshapes, outattrib);
            computeAllSmoothingNormals(outattrib, outshapes);
        }

        const std::vector<tinyobj::shape_t> &shapes = regen_all_normals ? outshapes : inshapes;
        const tinyobj::attrib_t &attrib = regen_all_normals ? outattrib : inattrib;

        {
            for (size_t s = 0; s < shapes.size(); s++)
            {
                objObject o;
                std::vector<float> buffer; // pos(3float), normal(3float), color(3float)

                // Check for smoothing group and compute smoothing normals
                std::map<int, vec3> smoothVertexNormals;
                if (!regen_all_normals && (hasSmoothingGroup(shapes[s])))
                {
                    computeSmoothingNormals(attrib, shapes[s], smoothVertexNormals);
                }

                for (size_t f = 0; f < shapes[s].mesh.indices.size() / 3; f++)
                {
                    tinyobj::index_t idx0 = shapes[s].mesh.indices[3 * f + 0];
                    tinyobj::index_t idx1 = shapes[s].mesh.indices[3 * f + 1];
                    tinyobj::index_t idx2 = shapes[s].mesh.indices[3 * f + 2];

                    int current_material_id = shapes[s].mesh.material_ids[f];

                    if ((current_material_id < 0) ||
                        (current_material_id >= static_cast<int>(materials.size())))
                    {
                        // Invaid material ID. Use default material.
                        current_material_id =
                            materials.size() -
                            1; // Default material is added to the last item in `materials`.
                    }

                    float diffuse[3];
                    for (size_t i = 0; i < 3; i++)
                    {
                        diffuse[i] = materials[current_material_id].diffuse[i];
                    }
                    float tc[3][2];
                    if (attrib.texcoords.size() > 0)
                    {
                        if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
                            (idx2.texcoord_index < 0))
                        {
                            // face does not contain valid uv index.
                            tc[0][0] = 0.0f;
                            tc[0][1] = 0.0f;
                            tc[1][0] = 0.0f;
                            tc[1][1] = 0.0f;
                            tc[2][0] = 0.0f;
                            tc[2][1] = 0.0f;
                        }
                        else
                        {
                            assert(attrib.texcoords.size() >
                                   size_t(2 * idx0.texcoord_index + 1));
                            assert(attrib.texcoords.size() >
                                   size_t(2 * idx1.texcoord_index + 1));
                            assert(attrib.texcoords.size() >
                                   size_t(2 * idx2.texcoord_index + 1));

                            // update tc (we don't need to flip Y for labVIEW)
                            tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
                            tc[0][1] = attrib.texcoords[2 * idx0.texcoord_index + 1];
                            tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
                            tc[1][1] = attrib.texcoords[2 * idx1.texcoord_index + 1];
                            tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
                            tc[2][1] = attrib.texcoords[2 * idx2.texcoord_index + 1];
                        }
                    }
                    else
                    {
                        tc[0][0] = 0.0f;
                        tc[0][1] = 0.0f;
                        tc[1][0] = 0.0f;
                        tc[1][1] = 0.0f;
                        tc[2][0] = 0.0f;
                        tc[2][1] = 0.0f;
                    }

                    float v[3][3];
                    for (int k = 0; k < 3; k++)
                    {
                        int f0 = idx0.vertex_index;
                        int f1 = idx1.vertex_index;
                        int f2 = idx2.vertex_index;
                        assert(f0 >= 0);
                        assert(f1 >= 0);
                        assert(f2 >= 0);

                        v[0][k] = attrib.vertices[3 * f0 + k];
                        v[1][k] = attrib.vertices[3 * f1 + k];
                        v[2][k] = attrib.vertices[3 * f2 + k];
                        bmin[k] = std::min(v[0][k], bmin[k]);
                        bmin[k] = std::min(v[1][k], bmin[k]);
                        bmin[k] = std::min(v[2][k], bmin[k]);
                        bmax[k] = std::max(v[0][k], bmax[k]);
                        bmax[k] = std::max(v[1][k], bmax[k]);
                        bmax[k] = std::max(v[2][k], bmax[k]);
                    }

                    float n[3][3];
                    {
                        bool invalid_normal_index = false;
                        if (attrib.normals.size() > 0)
                        {
                            int nf0 = idx0.normal_index;
                            int nf1 = idx1.normal_index;
                            int nf2 = idx2.normal_index;

                            if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0))
                            {
                                // normal index is missing from this face.
                                invalid_normal_index = true;
                            }
                            else
                            {
                                for (int k = 0; k < 3; k++)
                                {
                                    assert(size_t(3 * nf0 + k) < attrib.normals.size());
                                    assert(size_t(3 * nf1 + k) < attrib.normals.size());
                                    assert(size_t(3 * nf2 + k) < attrib.normals.size());
                                    n[0][k] = attrib.normals[3 * nf0 + k];
                                    n[1][k] = attrib.normals[3 * nf1 + k];
                                    n[2][k] = attrib.normals[3 * nf2 + k];
                                }
                            }
                        }
                        else
                        {
                            invalid_normal_index = true;
                        }

                        if (invalid_normal_index && !smoothVertexNormals.empty())
                        {
                            // Use smoothing normals
                            int f0 = idx0.vertex_index;
                            int f1 = idx1.vertex_index;
                            int f2 = idx2.vertex_index;

                            if (f0 >= 0 && f1 >= 0 && f2 >= 0)
                            {
                                n[0][0] = smoothVertexNormals[f0].v[0];
                                n[0][1] = smoothVertexNormals[f0].v[1];
                                n[0][2] = smoothVertexNormals[f0].v[2];

                                n[1][0] = smoothVertexNormals[f1].v[0];
                                n[1][1] = smoothVertexNormals[f1].v[1];
                                n[1][2] = smoothVertexNormals[f1].v[2];

                                n[2][0] = smoothVertexNormals[f2].v[0];
                                n[2][1] = smoothVertexNormals[f2].v[1];
                                n[2][2] = smoothVertexNormals[f2].v[2];

                                invalid_normal_index = false;
                            }
                        }

                        if (invalid_normal_index)
                        {
                            // compute geometric normal
                            calcNormal(n[0], v[0], v[1], v[2]);
                            n[1][0] = n[0][0];
                            n[1][1] = n[0][1];
                            n[1][2] = n[0][2];
                            n[2][0] = n[0][0];
                            n[2][1] = n[0][1];
                            n[2][2] = n[0][2];
                        }
                    }

                    for (int k = 0; k < 3; k++)
                    {
                        buffer.push_back(v[k][0]);
                        buffer.push_back(v[k][1]);
                        buffer.push_back(v[k][2]);
                        buffer.push_back(n[k][0]);
                        buffer.push_back(n[k][1]);
                        buffer.push_back(n[k][2]);
                        // Combine normal and diffuse to get color.
                        float normal_factor = 0.2;
                        float diffuse_factor = 1 - normal_factor;
                        float c[3] = {n[k][0] * normal_factor + diffuse[0] * diffuse_factor,
                                      n[k][1] * normal_factor + diffuse[1] * diffuse_factor,
                                      n[k][2] * normal_factor + diffuse[2] * diffuse_factor};
                        float len2 = c[0] * c[0] + c[1] * c[1] + c[2] * c[2];
                        if (len2 > 0.0f)
                        {
                            float len = sqrtf(len2);

                            c[0] /= len;
                            c[1] /= len;
                            c[2] /= len;
                        }
                        buffer.push_back(c[0] * 0.5 + 0.5);
                        buffer.push_back(c[1] * 0.5 + 0.5);
                        buffer.push_back(c[2] * 0.5 + 0.5);

                        buffer.push_back(tc[k][0]);
                        buffer.push_back(tc[k][1]);
                    }
                }

                o.vb_id = 0;
                o.numTriangles = 0;
                o.name = shapes[s].name;

                // OpenGL viewer does not support texturing with per-face material.
                if (shapes[s].mesh.material_ids.size() > 0 &&
                    shapes[s].mesh.material_ids.size() > s)
                {
                    o.material_id = shapes[s].mesh.material_ids[0]; // use the material ID
                                                                    // of the first face.
                }
                else
                {
                    o.material_id = materials.size() - 1; // = ID for default material.
                }

                if (buffer.size() > 0)
                {
                    o.numTriangles = buffer.size() / (3 + 3 + 3 + 2) / 3; // 3:vtx, 3:normal, 3:col, 2:texcoord
                    shapeVerticies.push_back(buffer);
                }
                drawObjects.push_back(o);
            }
        }

        return true;
    }
}

extern "C"
{
    G_AR_TOOLKIT_AR_EXPORT LV_MgErr_t g_ar_tk_ar_load_obj(
        LV_ErrorClusterPtr_t error_cluster_ptr,
        LV_StringHandle_t obj_path_handle,
        LV_StringHandle_t material_path_handle,
        LV_BooleanPtr_t regen_normals_ptr,
        LV_1DArrayHandle_t<LV_ObjObject_t> object_array_handle,
        LV_1DArrayHandle_t<LV_1DArrayHandle_t<LV_ObjPoint_t>> vertex_arrays_handle,
        LV_1DArrayHandle_t<LV_ObjMaterial_t> material_array_handle,
        LV_ObjectPointFloatPtr_t bb_min_ptr,
        LV_ObjectPointFloatPtr_t bb_max_ptr)
    {
        try
        {
            float bmin[3], bmax[3];
            std::vector<objObject> objects;
            std::vector<tinyobj::material_t> materials;
            std::vector<std::vector<tinyobj::real_t>> shape_verticies;

            bool success = load_obj_and_convert(bmin, bmax, objects, materials,
                                             obj_path_handle,
                                             material_path_handle,
                                             *regen_normals_ptr,
                                             shape_verticies);

            if (success)
            {
                // copy objects
                object_array_handle.copy_element_by_element_from(objects, [](auto from, auto to)
                                                                 {
                    to->name_handle.copy_from(from.name);
                    to->material_id =  from.material_id; });

                // copy verticies
                vertex_arrays_handle.copy_element_by_element_from(shape_verticies, [](auto from, auto to)
                                                                  {
                                                   // std::vector is a long list of values which have the same format as LV_ObjPoint_t

                                                   size_t n_bytes = from.size() * sizeof(tinyobj::real_t);
                                                   size_t n_elements = n_bytes / sizeof(LV_ObjPoint_t);

                                                   to->size_to_fit(n_elements);

                                                   auto it_from = from.begin();
                                                   auto it_to = to->begin();

                                                   for (; it_from != from.end() && it_to!=to->end(); ++it_from, ++it_to)
                                                   {
                                                        // LV_ObjPoint_t has an overloaded assignment operator
                                                        // to make this work
                                                       *it_to = it_from;
                                                   } });

                // copy materials
                material_array_handle.copy_element_by_element_from(materials);

                // set bouding box
                *bb_min_ptr = bmin;
                *bb_max_ptr = bmax;
            }
        }
        catch (...)
        {
            error_cluster_ptr.copy_from_exception(std::current_exception(), __func__);
        }
        return LV_ERR_noError;
    }
}