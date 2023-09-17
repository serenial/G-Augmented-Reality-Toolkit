#include <vector>

#include "g_ar_toolkit_export.h"
#include "g_ar_toolkit/capture/context.hpp"
#include "g_ar_toolkit/capture/stream.hpp"


extern "C"
{
  G_AR_TOOLKIT_EXPORT void g_ar_tk_list_devices()
  {

    try
    {
      auto ctx = g_ar_toolkit::capture::create_platform_context();

      std::vector<g_ar_toolkit::capture::device_info_t> devices;
      ctx->enumerate_devices(devices);

      delete (ctx);
    }
    catch (std::exception &e)
    {
      auto b = e.what();
    }
    catch (...)
    {
      // somthing else
      auto z = 1;
    }
  }

    G_AR_TOOLKIT_EXPORT void g_ar_tk_create_stream()
  {

    try
    {
      auto ctx = g_ar_toolkit::capture::create_platform_context();

      std::vector<g_ar_toolkit::capture::device_info_t> devices;
      ctx->enumerate_devices(devices);
      auto strm = ctx->open_stream(devices[0].device_id,devices[0].supported_formats[0]);
      delete (strm);
      delete (ctx);
    }
    catch (std::exception &e)
    {
      auto b = e.what();
    }
    catch (...)
    {
      // somthing else
      auto z = 1;
    }
  }
}