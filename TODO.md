# Apeal TODO
---

## BUGS
- Yaml parser.
  - Fix nested tripple arrays: [[[1,2,3], [4,5,6]]]
  - Fix line breaking of long nested arrays.
  - Fix eof at the end of the text
  - Doesn't parse properly if (pentix nova levels):
    - maps:
      - item 1
      - item 2
  - Doesn't properly parse quoted string if it's a value (pentix levels rows quoted)

- Tests parseInt int64_t
- Tests string escape
- Tests string format fmt_custom_type
- Tests Thread event
- Tests net http

## FIXES

## Documentation
- Documentation in GitHub format


## Feature
- Use pkgconfig for configure scripts
- implement "import-3rdparty" scripts to automatically configure and copy 3rd party packages
- Amalgamation
- Implement and test all arythmetic operators for Size, Point, Rect, Margin

- Implement base64
- Rename Crc to Checksum. Implement Fletcher32, Fletcher64
- Implement CRC-1 (parity), CRC-8, CRC-8-GSM, CRC-16-CCITT, CRC-16-ANSI, CRC-32, CRC-64-ISO
 
- Implement DebugEval(expr) to evaluate the expression only in debug mode. 

- investigate trapping floating point exception (throw error)

- study c++20 <format> library 

- consider in the future std::invoke for signal/slot <functional>

- implement something equivalent to std::quoted (for producing quoted strings)


- implement Indices(iterating produces std::size_t or int indices) 
    and ZipRange (returns tuples of the containers)
    for (auto& [a,b]: ZipRange{ac, bc}) {}

- Buffer:
  - Implement static buffer (like std::array) but copyable to from Buffer
  - implement emplace_back and test it
  - Implement allocator: std type
  - add string functionality, mimick std::string
    - c_str() (just return the data)
    - length() - return size()
  - implement operator += for element
  - implement the proper way for element that require constructor arguments

- lang:
  - Change all the string related stuff to `constexpr` when MS STL implements `constexpr std::string`
  - Change all the functions that accept `StrView` to `Str` with concepts when concepts are implemented everywhere

- string:
  - Implement fully conformant String class with small string optimization


### Images

- Implement pgm files
- Inquire list of supported formats, extensions, mime-types


### Parsers

- yaml:
  - Test for i18n
  - Implement tags !!binary, !!float etc

- json:
  - Have only 3 types of nodes: node, array, value
  - Array is always string
  - Add `underlying` parameter to the array node: string, integer, float
  - Implement `begin()` `end()` members for the nodes regardless of type
  - Implement `get<T>()` and `get<T>(T def)` members to get the values.

- ini:
  - add list type values
  - add error if missing option
  - add "continue on new line" character (which is '\')
  - add escape character (which is '\')
  - do the tree structure - not flat

- xml:
  - parse comments into content


### Threading


### Tests
- Fix: thread/event
- Fix: system/info
- Fix: signal
- Fix: process
- Fix: parse/url (g:h throws error: URI doesn't end correctly)
- Fix: parse/preprocessor
- Fix: parse/csv
  