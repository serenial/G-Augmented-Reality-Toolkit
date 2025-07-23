// set the packing and disable the MSVC warning
// G_AR_TOOLKIT_BYTE_PACKING_4 defined at compile time

#ifdef G_AR_TOOLKIT_BYTE_PACKING_4
#pragma pack(push, 1)
#pragma warning (disable : 4103)
#endif