.. _hashing_design:

##############
Hashing Design
##############

At the moment hashing is not needed by ParallelZone, but will be needed
downstream. Ultimately our recommendation is to use the hash function built into
C++. This page exists mainly to document the considerations that went into this
design decision. This documentation lives in ParallelZone (as opposed to
downstream) mainly because the hash implementation is similar to how
serialization is implemented (i.e., registering a class's attributes) and our
hope is that by considering it here we can reduce boilerplate between hashing
and serialization.

****************
What is Hashing?
****************

Hashing is the process of mapping an arbitrarily large data object to a
relatively small, fixed-size data object. The fixed-size data object is known as
a "hash value" or just "hash". In practice, hashes are usually 32 byte integers
(not to be confused with the commonly used 32 *bit* integer) which are usually
represented as a 64 digit string of hexadecimal integers. The actual mapping
from the input object to the hash is done by a "hash function".

Over the years a large number of hash functions have been proposed, each with
their own properties. For our purposes we are most interested in hash functions
which have the following properties:

- (approximately) injective

   - An injective function maps each unique input in the domain to a unique
     output in the function's image
   - A truly injective hash function is only possible if the size of the
     function's image is greater than or equal to the size of the domain.
   - In practice the domain of a hash function is almost always larger than the
     function's image.
   - An approximately injective hash function is one which, for all intents
     and purposes, is injective because it is extremely unlikely that any two
     elements of the domain will map to the same element of the image
   - Exactly how unlikely hash collisions need to be in order for a hash
     function to be considered approximately injective varies with the
     application; however, hash collisions are so rare with most modern hash
     functions that they are usually approximately injective for most
     applications.

- efficiency

   - Computing the hash should be relatively quick

***********************
Why Do We Need Hashing?
***********************

We need hashing primarily as a means of quickly comparing instances of the
same class. If applying a hash function to two objects of that class results in
two different hashes, then the objects are guaranteed to be different. If
instead the hash function maps the two objects to the same hash, and assuming
the hash function is approximately injective, then it is extremely likely that
the objects are the same. It is worth noting that these statements assume that
hashing of the class is implemented correctly (e.g., hashing of an attribute is
not overlooked).

A related use case is related to hash tables. A hash table is a mapping from a
hash to an object. The hash used as a key is typically derived from a different
object than the value the hash maps to. Hash tables are typically used to speed
up value look up or when  storing the actual keys is expensive.

**********************
Hashing Considerations
**********************

- Approximately injective hash function

   - Most of our applications work best when the hash function is approximately
     injective.
   - Ultimately, it's up to the developer to decide if the approximate
     injective-ness of the hash function suffices and/or to have a back up plan
     for collisions.

- Can hash arbitrary classes

   - C++ lacks reflection so this can not be done automatically, rather the
     design needs to include some mechanism for the hash function to discover
     the attributes of a class.

- Will need to hash distributed classes

   - Again C++ has no reflection so this can not be done automatically.
   - Likely will want two versions: a hash for just the local state and a hash
     for the entire distributed object.
   - Implementing these hash scenarios will ultimately be up to the developer.

- Will need to hash polymorphic classes

   - In some scenarios we only want to hash the visible state (state included by
     the type of the object plus state from all base classes)
   - Other times we want a hash of the entire state (the visible state plus any
     state in classes derived from the current type)

***********************
Hashing Implementations
***********************

Since C++11, the C++ standard libraries come with hash support. This section
lists external C++ hashing implementations.

- boost::hash

   - https://github.com/boostorg/container_hash
   - Boost has had ``boost::hash`` for a while.
   - It appears that this is now part of the container hash component
   - It also appears that container hash is only kept around for backwards
     compatibility purposes
   - BSL v1
   - That said the GitHub page appears to be under active development
   - 4.8K stars and 288 watchers (Boost super project)
   - 8 stars and 9 watchers (container_hash alone)

- BPHash

   - https:://github.com/bennybp/bphash
   - Provides a consistent and portable (with some caveats) hash function
   - C++ standard hash function is not guaranteed to be consistent between runs
     or portable
   - BSD
   - Abandoned
   - 3 stars and 5 watchers

- Hash-library

   - https://github.com/stbrumme/hash-library
   - Provides a number of different hash functions
   - Requires the user to serialize their data in one of three ways: streaming
     the data into the hash, hashing a string representation, or hashing a
     vectorized format of the data
   - Zlib License
   - Last commit September 2021
   - 220 stars and 10 watchers

.. todo::

   This list is unlikely to be an exhaustive set of all C++ hashing libraries
   and should be revisited at some to make it more exhaustive.

****************
Hashing Strategy
****************

None of our current hashing use cases seem to justify using hashing beyond that
afforded by the C++ standard library. So for now we simply rely on the hashing
capabilities of the C++ standard library, extending it to our custom types.
