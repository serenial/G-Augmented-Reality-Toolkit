#include "usb_cam/conversions.hpp"

namespace usb_cam
{
namespace conversions
{


std::string FCC2S(const unsigned int & val)
{
  std::string s;

  s += val & 0x7f;
  s += (val >> 8) & 0x7f;
  s += (val >> 16) & 0x7f;
  s += (val >> 24) & 0x7f;
  if (val & (1 << 31)) {
    s += "-BE";
  }
  return s;
}

}  // namespace conversions
}  // namespace usb_cam