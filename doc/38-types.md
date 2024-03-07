DBus Types {#dbus-types}
===

DBus-cxx supports all DBus types.  The following table summarizes the type in C++ and in DBus:

|DBus Type(char)|C++ Type|
|---------------|--------|
|BYTE(`y`)      |`uint8_t`|
|BOOLEAN(`b`)   |`boolean`|
|INT16(`n`)     |`int6_t`|
|UINT16(`q`)    |`uint16_t`|
|INT32(`i`)     |`int32_t`|
|UINT32(`u`)    |`uint32_t`|
|INT64(`x`)     |`int64_t`|
|UINT64(`t`)    |`uint64_t`|
|DOUBLE(`d`)    |`double`|
|STRING(`s`)    |`std::string`|
|OBJECT\_PATH(`o`)|[DBus::Path](@ref DBus.Path)|
|SIGNATURE(`g`) |[DBus::Signature](@ref DBus.Signature)|
|ARRAY(`a`)     |`std::vector`|
|STRUCT(Note 1)       |`std::tuple`|
|VARIANT(`v`)   |[DBus::Variant](@ref DBus.Variant)|
|DICT\_ENTRY(Note 2)|`std::map`|
|UNIX\_FD(`h`)  |[DBus::FileDescriptor](@ref DBus.FileDescriptor)|

**Note 1:** Structs use `()` on the bus as their signature - meaning `(i)` is a struct containing one INT32, `(dib)` is a struct containing a DOUBLE, INT32, BOOLEAN.

**Note 2:** In DBus, dicts are shown as arrays with curly brackets.  So `a{is}` is a map of integers to strings: `std::map<int32_t,std::string>`
