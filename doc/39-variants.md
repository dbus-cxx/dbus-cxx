Variants {#variants}
===
Variants are implemented in DBus-cxx as their own class, [Variant](@ref DBus.Variant).
Variants are type-safe unions that can contain a single type in them.

There are some limitations with Variants with how DBus-cxx implements them:

**Variants cannot be deeply nested**
Variants are only guaranteed to be parsed if they are no more than two deep. For example,
a Variant that contains a `map<string,Variant>` can be parsed, but if one of those
variants inside of the map contains another variant it is unlikely to work.

**Variants are immutable**
Variants as they are currently implemented are immutable and cannot be changed once they
are created.  If you need to change the value, you must create a new variant.

These limitations are at least partially due to how C++ templates work.  Because
we need to know at extraction time what the type is, this must all be implemented
in the header, but then we get into a situation where we have a bit of a chicken-and-egg problem.
Because variants are also stored by DBus-cxx as their marshaled format, this makes it
very difficult to fully extract anything.

If you are interested in helping to fix this, feel free to create a Github discussion
in order to give your ideas on how to fix!  Patches would also be welcomed.

