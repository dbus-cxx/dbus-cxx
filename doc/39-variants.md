Variants {#variants}
===
Variants are implemented in DBus-cxx as their own class, [Variant](@ref DBus.Variant).
Variants are type-safe unions that can contain a single type in them.

As of version 2.5.0, variants should be able to be nested to an arbitrary depth.

There are some limitations with Variants with how DBus-cxx implements variants:

**Variants are immutable**
Variants as they are currently implemented are immutable and cannot be changed once they
are created.  If you need to change the value, you must create a new variant.

