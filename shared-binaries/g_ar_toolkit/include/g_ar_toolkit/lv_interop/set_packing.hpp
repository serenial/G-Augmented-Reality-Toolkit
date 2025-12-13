// set the packing and disable the MSVC warning
// G_AR_TOOLKIT_BYTE_PACKING_WIN_x86 defined at compile time

#ifdef G_AR_TOOLKIT_BYTE_PACKING_WIN_x86
#pragma pack(push, 1)
#pragma warning (disable : 4103)
#endif