#ifndef G_AR_TK__INTEROP_LV_STR_HPP_
#define G_AR_TK__INTEROP_LV_STR_HPP_

#include <string>
#include <string_view>

#include "./lv_types.hpp"
#include "./lv_array_1d.hpp"

namespace g_ar_toolkit
{
    namespace lv_interop
    {
        class LV_StringHandle_t : public LV_1DArrayHandle_t<char>{
            public:
            operator std::string_view()  const;
            operator const std::string()  const;
            void copy_from(const std::string&);
        };


    }

    
}

#endif //G_AR_TK__INTEROP_LV_STR_HPP_