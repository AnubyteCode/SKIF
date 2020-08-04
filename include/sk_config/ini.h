//
// Copyright 2020 Andon "Kaldaien" Coleman
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
//
#ifndef __SKIF__INI_H__
#define __SKIF__INI_H__

#include <string>
#include <unordered_map>
#include <vector>

#include <Unknwnbase.h>

// {B526D074-2F4D-4BAE-B6EC-11CB3779B199}
static const GUID IID_SK_INISection =
{ 0xb526d074, 0x2f4d, 0x4bae, { 0xb6, 0xec, 0x11, 0xcb, 0x37, 0x79, 0xb1, 0x99 } };

interface iSK_INI;

interface iSK_INISection : public IUnknown
{
//friend interface iSK_INI;

public:
  iSK_INISection (void) = default;

  iSK_INISection (const wchar_t* section_name) : name   (section_name),
                                                 parent (nullptr) {
  }

  iSK_INISection (const std::wstring& sec_name) : name   (sec_name),
                                                  parent (nullptr) {
  }

  iSK_INISection ( const wchar_t* section_name,
                         iSK_INI* _parent )     : name   (section_name),
                                                 parent (_parent) {
  }

  iSK_INISection ( const std::wstring& sec_name,
                              iSK_INI* _parent) : name   (sec_name),
                                                  parent (_parent) {
  }

  virtual ~iSK_INISection (void) noexcept { };

  /*** IUnknown methods ***/
  STDMETHOD  (       QueryInterface)(THIS_ REFIID riid, void** ppvObj);
  STDMETHOD_ (ULONG, AddRef)        (THIS);
  STDMETHOD_ (ULONG, Release)       (THIS);

  STDMETHOD_ (std::wstring&, get_value)    (const wchar_t* key);
  STDMETHOD_ (void,          set_name)     (const wchar_t* name_);
  STDMETHOD_ (bool,          contains_key) (const wchar_t* key);
  STDMETHOD_ (void,          add_key_value)(const wchar_t* key, const wchar_t* value);
  STDMETHOD_ (bool,          remove_key)   (const wchar_t* key);

  //protected:
  //private:
  std::wstring                                    name;
  std::unordered_map <std::wstring, std::wstring> keys;
  std::vector        <std::wstring>               ordered_keys;
  iSK_INI*                                        parent;

  ULONG                                           refs = 0;
};

// {DD2B1E00-6C14-4659-8B45-FCEF1BC2C724}
static const GUID IID_SK_INI =
{ 0xdd2b1e00, 0x6c14, 0x4659, { 0x8b, 0x45, 0xfc, 0xef, 0x1b, 0xc2, 0xc7, 0x24 } };

interface iSK_INI : public IUnknown
{
  friend interface iSK_INISection;

  using _TSectionMap =
    std::unordered_map <std::wstring, iSK_INISection>;

           iSK_INI (const wchar_t* filename);
  virtual ~iSK_INI (void);

  /*** IUnknown methods ***/
  STDMETHOD  (       QueryInterface)(THIS_ REFIID riid, void** ppvObj);
  STDMETHOD_ (ULONG, AddRef)        (THIS);
  STDMETHOD_ (ULONG, Release)       (THIS);

  STDMETHOD_ (void, parse)  (THIS);
  STDMETHOD_ (void, import) (THIS_ const wchar_t* import_data);
  STDMETHOD_ (void, write)  (THIS_ const wchar_t* fname);

  STDMETHOD_ (_TSectionMap&,   get_sections)    (THIS);
  STDMETHOD_ (iSK_INISection&, get_section)     (const wchar_t* section);
  STDMETHOD_ (bool,            contains_section)(const wchar_t* section);
  STDMETHOD_ (bool,            remove_section)  (const wchar_t* section);

  STDMETHOD_ (iSK_INISection&, get_section_f)   ( THIS_ _In_z_ _Printf_format_string_
                                                  wchar_t const* const _Format,
                                                                       ... );
  STDMETHOD_ (const wchar_t*,  get_filename)    (THIS) const;
  STDMETHOD_ (bool,            import_file)     (const wchar_t* fname);
  STDMETHOD_ (bool,            rename)          (const wchar_t* fname);

protected:
private:
  FILE*     fINI    = nullptr;

  wchar_t*  wszName = nullptr;
  wchar_t*  wszData = nullptr;
  wchar_t*     data = nullptr; // The original allocation base
       //  (wszData may be offset against a Byte Order Marker)

  std::unordered_map <
    std::wstring, iSK_INISection
  >         sections;

  // Preserve insertion order so that we write the INI file in the
  //   same order we read it. Otherwise things get jumbled around
  //     arbitrarily as the map is re-hashed.
  std::vector <std::wstring>
            ordered_sections;

  // Preserve File Encoding
  enum CharacterEncoding {
    INI_INVALID = 0x00,
    INI_UTF8    = 0x01,
    INI_UTF16LE = 0x02,
    INI_UTF16BE = 0x04 // Not natively supported, but can be converted
  } encoding_;

  ULONG    refs_  = 0;
  uint32_t crc32_ = 0; // Skip writing config files that haven't changed
};

iSK_INI*
__stdcall
SKIF_CreateINI (const wchar_t* const wszName);

#endif /* __SKIF__INI_H__ */