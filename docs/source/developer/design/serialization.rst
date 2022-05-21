.. _serialization_design:

####################
Serialization Design
####################

**********************
What is Serialization?
**********************

In the context of object-oriented programming, serialization is the process of
deconstructing an object's state into a flat state that is suitable for storage
and/or transmission. For most applications it is also important that it be
possible to recreate the object's initial state from the flattened state, a
process known as deserializing. Unless specified otherwise, on this page, when
we refer to "serialization" this includes both serializing and deserializing.

C++ lacks reflection (the ability for a class to examine, introspect, and modify
its own structure), which in turn means it is not possible for serialization to
occur automatically for an arbitrary C++ class. Instead developers need to
somehow write serialization routines for their objects. The exact requirements
for the serialization routines depend on the library used for serialization, but
in general it amounts to a recursive process where each class loops over its
members and serializes/deserializes them (recursion terminating when fundamental
C++ types are found).

*****************************
Why Do We Need Serialization?
*****************************

In ParallelZone we need serialization for transmitting objects. More
specifically in order to be able send/receive objects we need to be able to
serialize/deserialize them. We anticipate that downstream dependencies may want
to piggy back off of the ParallelZone serialization infrastructure to store
objects.

.. _serialization_considerations:

****************************
Serialization Considerations
****************************

This section lists the points which were considered in devising our
serialization solution.

- Support for C++ standard library types

   - While we could implement our own serialization routines for the classes
     found in the C++ standard library, many implementations already do so.
   - Using a serialization library that already supports the standard library
     is a preferred, but not a deal breaker.

- Need to serialize ``std::shared_ptr``

   - Serializing ``std::shared_ptr`` entails not only serializing the object
     being pointed to, but also the shared semantics (reference count and who
     has access to the shared object)
   - Deserializing should produce one copy of the instance and restore the
     shared semantics
   - Technically falls under above consideration, but given that it's more
     complicated than just serializing the state of the pointer we called it
     out.

- Need to serialize polymorphic types

   - Many of our classes rely on polymorphism for some sort of type-erasure
   - Polymorphism means we may try to serialize via a pointer/reference to the
     base class
   - Virtual serialization methods allow C++ to handle the polymorphism aspect
     automatically; however, many serialization libraries require the
     serialization methods to be templated on the object doing the serialization
     (so they can't be virtual)
   - Registration of derived classes is a common solution
   - Can also write our own ``Serializer`` class so function doesn't need to be
     templated.

- Multiple serialization formats

   - Serializing to binary is nice for transmitting and short-term storage of
     data.
   - Human-readable formats are nice for long-term storage and inter-program
     communication.
   - The ideal solution would have a mechanism to extend the formats it can
     serialize to

- Need to perform serialization on objects we do not control the source code for

   - Need the option (even if it ends up being more expensive) to do
     serialization through an object's public API

- Serializing distributed objects

   - ParallelZone encourages a SPMD coding philosophy
   - SPMD means some of the objects we need to serialize will have distributed
     state
   - Without reflection, there's no general way to know how the object is
     distributed and the object developer will need to be involved in the
     process
   - In some circumstances we will only want to serialize the local state,
     whereas in other cases we'll want to serialize all of the state
   - Serialization libraries are unlikely to help with this aspect

- Serializing objects with special save/load requirements

   - Sometimes there's more to serialization than just serializing the
     attributes of an object (e.g., needing to redistribute the data in a
     distributed object)

*****************************
Serialization Implementations
*****************************

This section is a collection of the C++ serialization libraries for which we are
aware. For each library we have provided some basic vitals and some notes. A
detailed comparison of the more promising choices are given below.

- Cereal

   - https://github.com/USCiLab/cereal
   - Support for serializing arbitrary C++ objects into multiple backends
   - BSD License
   - Looked abandon for a long time, but seems to have sprung back to life
     recently.
   - 3.1K stars and 152 watchers

- MADNESS

   - https://github.com/m-a-d-n-e-s-s/madness
   - More or less a hand-rolled version of Boost serialization?
   - Already in ParallelZone (largely b/c TiledArray uses it)
   - GPLv2
   - Active development
   - 125 stars and 30 watchers.

- Boost

   - https://github.com/boostorg/serialization
   - Pre-C++11 was a widely used serialization library; however, it does not
     look like it kept up with the STL since then.
   - BSLv1
   - Active development
   - 4.8k stars and 288 watchers (for the Boost super project).

- Cista

   - https://github.com/felixguendling/cista
   - Provides a series of types that have reflection
   - Serialization appears to work by building your classes with the reflective
     types.
   - Appears to have support for writing your own serialization functions when
     you don't use their types, but probably not the primary use case
   - MIT License
   - Active development.
   - 824 stars and 27 watchers.

- zpp serializer

  - https://github.com/eyalz800/serializer
  - Conceptually appears similar to Boost/Cereal
  - For polymorphism requires base type to inherit from a zpp class, which is
    problematic if you don't control the class.
  - MIT License
  - Last commit December 2021.
  - 140 stars and 8 watchers.

- zpp::bits

  - https://github.com/eyalz800/zpp_bits
  - Successor to zpp serializer
  - Requires C++20
  - MIT License
  - Active development.
  - 161 stars and 2 watchers.

- HPS

   - https://github.com/jl2922/hps
   - Designed as a faster alternative to existing serialization libraries
   - MIT License
   - Active development
   - 55 stars and 7 watchers

- cereal_fwd

   - https://github.com/breiker/cereal_fwd
   - Fork of Cereal which adds forward compatibility and portability
   - A slightly modified BSD license
   - Likely abandoned (last commit September 2017)
   - 1 star and 1 watcher

- Protocol Buffers

  - https://github.com/protocolbuffers/protobuf
  - Requires you to write a schema for your class and then compile it.
  - From what I can tell it basically builds reflection into the language
  - Recommended as PIMPLs
  - Looks like a modified form of the Apache v2 License (?)
  - Active development.
  - 53.5K stars and 2.1K watchers.

- Cap'n Proto

  - https://github.com/capnproto/capnproto
  - Written by primary author of Protocol Buffers
  - Marketed as a faster version of Protocol Buffers
  - MIT License
  - Active development
  - 8.8K stars and 318 watchers.

- FlatBuffers

   - https://github.com/google/flatbuffers
   - Faster version of Protocol Buffers from Google
   - Apache v2
   - Active development.
   - 17.6K stars and 642 watchers.

- msgpack-c

  - https://github.com/msgpack/msgpack-c
  - C++ library for serializing/deserializing to/from MessagePack format

     - MessagePack is a standardized binary serialization format

  - Presumably the official C/C++ library for MessagePack
  - BSL v1
  - Active development
  - 2.4K stars and 160 watchers

- msgpack11

   - https://github.com/ar90n/msgpack11
   - C++11 library which can process MessagePack
   - MIT License
   - Likely abandon (last commit July 2018)
   - 86 stars and 11 watchers.

- tser

   - https://github.com/KonanM/tser
   - Specifically designed to be a small (in size) serialization library
   - Behavior-wise similar to Boost/Cereal
   - BSL v1
   - Last commit May 2021
   - 91 Stars and 4 watchers.

- Bitsery

   - https://github.com/fraillt/bitsery
   - Cereal/Boost-like API, no macros
   - MIT License
   - Last commit October 2021
   - 666 stars and 27 watchers.

- Yet Another Serialization (YAS)

   - https://github.com/niXman/yas
   - Meant as a replacement for Boost
   - BSL v1
   - Active development
   - 519 stars and 37 watchers.

- Avro

  - https://github.com/apache/avro
  - Requires external schemas to serialize/deserialize
  - Apache v2
  - Active development
  - 2.1K stars and 106 watchers

- Thrift

  - https://github.com/apache/thrift
  - Looks like it's more worried about RPC than serialization directly
  - Apache v2
  - Active development
  - 9K stars and 463 watchers

- s11n

  - s11n.net
  - It doesn't appear to be possible to see the APIs without downloading a tar
    ball
  - License is just listed as "public domain"
  - Source hasn't been updated since 2013 (website was updated in 2021)
  - Likely considered feature complete

.. note::

   As a disclaimer the author does not have experience with most of these
   libraries (aside from Cereal and Boost Serialization). In these cases
   assessments are made by looking at the documentation and may or may not
   accurately reflect the actual state of the library. Statistics pertaining to
   activity, stars, and watchers were accurate as of March 2022 and have likely
   changed since then.

Detailed Comparison
===================

Based on the discussion in :ref:`serialization_considerations`, we have
identified the following features we are looking for in a serialization library:

Matureness

   The library should be mature (e.g. have a stable API, decent adoption, well
   documented). Without actually using all of the libraries, consulting their
   communities, this is admittedly a judgment call based off of first
   impressions and GitHub metrics (we were looking for more than 500 stars) than
   anything else.

Compatibly Licensed

   We use Apache v2. The library must have a license which is compatible with
   that license. With the exception of MADNESS (and possibly s11n), all
   serialization libraries are licensed in a way we could use them directly.

STL

   We would prefer to not have to roll our own serializations for classes found
   in the STL. Admittedly this is a moving target (new classes are added as new
   C++ standards come out); however, if a library supports classes added in
   C++11 (or newer) that's good enough.

shared_ptr

   Preserves ``std::shared_ptr`` semantics.

OOP

   The library ideally will have some mechanism for supporting object
   hierarchies (allowing the serialization methods to be virtual counts as does
   registering relationships).

Formats

   Should be possible to add additional serialization formats.

Schemaless

   We do not want to maintain separate schema files for our classes. Typically
   said files need to be processed by a separate tool as part of the build
   process, which adds complexity and causes the build process to deviate from
   the usual procedure.

3rd Party

   We will need to be able to serialize objects that we didn't write and thus
   we won't be able to modify the source of.

The following table compares the serialization libraries on these features. For
simplicity the table does not include libraries which are deemed to fail the
"matureness" criteria (in most cases because there wasn't enough interest).

.. |x| replace:: ❌
.. |o| replace:: ✅


+------------------+-----+------------+-----+---------+------------+-----------+
| Name             | STL | shared_ptr | OOP | Formats | Schemaless | 3rd Party |
+==================+=====+============+=====+=========+============+===========+
| Cereal           | |o| | |o|        | |o| | |o|     | |o|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| Boost            | |x| | |x|        | |o| | |o|     | |o|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| Cista            | |x| | |x|        | |x| | |x|     | |o|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| Protocol Buffers | |x| | |x|        | |x| | |o|     | |x|        | |x|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| FlatBuffers      | |x| | |x|        | |x| | |x|     | |x|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| msgpack-c        | |o| | |x|        | |x| | |x|     | |o|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| Bitsery          | |x| | |x|        | |x| | |x|     | |o|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| YAS              | |x| | |x|        | |x| | |x|     | |o|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+
| Avro             | |x| | |x|        | |o| | |x|     | |x|        | |o|       |
+------------------+-----+------------+-----+---------+------------+-----------+

.. note::

   We marked a library as not having a feature if the documentation did not
   discuss the feature (or explicitly stated that such a feature does not
   exist). Conversely we said a library has a feature if the documentation
   discusses it. This means that it is possible that a library's features may
   differ from those shown above based on the accuracy/completeness of its
   documentation.

.. _serialization_strategy::

**********************
Serialization Strategy
**********************

On the basis of the head-to-head serialization library comparisons, Cereal
appears to be the clear winner. While Cereal is currently being actively
developed, this is a somewhat recent development. More specifically there was
an over two year gap in between v1.3.0 and v1.3.1. Given the popularity of the
project, there's reason to be optimistic that Cereal will continue to be
supported into the future. Nonetheless, we feel it is prudent to ensure that
our external serialization library is as decoupled from our stack as possible.

Decoupling the external serialization library from our classes is difficult on
account of the fact that C++ lacks reflection. Since there is no reflection,
class developers must ultimately expose their members to the serialization
library. The serialization library, for its part, must be able to accept members
of arbitrary type. The former means the class must have the API the
serialization library expects, whereas the latter means the serialization
function must be templated (and thus can't be virtual). It's worth noting that
hashing runs into the same problems, for the same reason.

Section :ref:`reflection_design` explores the possibility of adding reflection
to our classes. The verdict there is that there does not appear to be any
existing solution which meets our needs. This means we would have to write our
own reflection library if we want to use it. While having reflection at our
disposable would be extremely useful, adding it in a robust manner will likely
require much more time than simply using Cereal directly. Our solution is thus
to require that each object is serializable with Cereal directly.

Cereal by itself already covers most of our considerations. The notable
exception is the treatment of distributed objects. In particular we need a
mechanism to be able to distinguish between when we want to serialize only the
local state and when we want to serialize all of the state. For now our
opinion is that serialization should by default only be the local state. When we
need to serialize all of the state we will create a new Cereal archive and
classes can dispatch based on whether they get that new archive or an already
existing one.

Below we briefly summarize how our solution addresses the considerations raised
above.

- Support for C++ standard library types

   - Handled by Cereal

- Need to serialize ``std::shared_ptr``

   - Handled by Cereal

- Need to serialize polymorphic types

   - Handled by Cereal

- Multiple serialization formats

   - Cereal allows us to write new archives

- Need to perform serialization on objects we do not control the source code for

   - Handled by Cereal

- Serializing distributed objects

   - For now serialization should only be the local piece of the object. A
     special archive will be written for when the entire state should be
     written to the archive. Classes can dispatch based on archive type.

- Serializing objects with special save/load requirements

   - Handled by Cereal
