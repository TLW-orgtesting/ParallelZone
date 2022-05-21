.. _reflection_design:

#################
Reflection Design
#################

This page describes the considerations that went into the reflection API
ParallelZone relies on.

*******************
What is Reflection?
*******************

In object-oriented languages one distinguishes between the object's type and the
object's state (i.e., class vs. instance of a class). While most coding
scenarios focus on inspecting and manipulating an object's state, there are
times (usually when writing metafunctions) when the developer wants to inspect
and manipulate the object's class. A language is said to contain reflection if
at runtime it is possible to inspect the object's class, and to manipulate it.
Admittedly, the manipulation part is usually limited in statically typed
languages like C++.

At the time of writing, C++ has no native support for reflection. That said,
there are proposals to add reflection to the C++ language in an upcoming
standard; however, it is likely that the earliest standard which could include
reflection would be the one after C++23 (C++26?). Point being, reflection as
part of the C++ language is a ways away.

**************************
Why Do We Need Reflection?
**************************

Strictly speaking we don't need reflection, but the lack of reflection leads to
a lot of boilerplate when adding meta-operations (e.g. printing, value
comparison, hashing, and serialization). These operations all involve some form
of "for each attribute, do...".  Since C++ doesn't include reflection, the usual
solution is to hard-code the list of attributes into each operation. This works,
but it makes switching say hashing or serialization libraries a pain.
With reflection, we can keep interactions with such libraries in a single place.

*************************
Reflection Considerations
*************************

- Need to reflect polymorphic classes

   - Solution must be aware that a given object may actually have been declared
     as a more derived class than how it presently is being viewed, e.g., the
     function may be given a reference/pointer to a base class of the object

- Need to reflect classes that use PIMPL

   - Only some of the state of a class is visible from the API exposed in the
     declaration.
   - Reflection of the PIMPL will still occur, but it will only be visible from
     source files.

- Need to reflect classes we don't control the source for

   - Third-party classes, such as those in the STL will need to have
     reflection added to them from outside of their definitions

- Need to reflect constructors

   - Notable because constructors behave differently than other member functions

**************************
Reflection Implementations
**************************

Below is a list of C++ libraries implementing reflection without outside tools
such as schemas or code generation.

- Boost::Describe

   - https://github.com/boostorg/describe
   - Members are registered with a macro
   - Inspections return a list of "traits" classes
   - The "traits" classes have members which are accessible at both compile and
     runtime.
   - BSLv1
   - 4.8K stars and 289 watchers (Boost super project)
   - 30 stars and 6 watchers (Describe alone)

- Run Time Type Reflection (RTTR)

   - https://github.com/rttrorg/rttr
   - More verbose registration process than Boost::Describe, but also more
     features
   - Registration happens at runtime, so can't be used at compile time
   - MIT License
   - Last commit August 2021
   - 2.1K stars and 89 watchers

- CAMP

   - https://github.com/tegesoft/camp
   - MIT License
   - Unmaintained
   - 81 stars and 17 watchers

- CAMP (fork)

   - https://github.com/fw4spl-org/camp
   - Recommended fork of CAMP
   - Abandoned
   - 4 stars and 5 watchers.

- Ponder

   -https://github.com/billyquith/ponder
   - Another fork of CAMP, but then massively overhauled
   - API is very similar to RTTR
   - MIT License
   - Likely abandoned (Last commit Dec 2020).
   - 519 stars and 31 watchers

.. note::

   Statistics and assessments were accurate as of March 2022. The author does
   not have any experience with any of these libraries and notes are based off
   quickly perusing documentation and source code.

*******************
Reflection Strategy
*******************

Of the available libraries only Boost::Describe and RTTR are currently
supported. AFAIK, RTTR is only runtime reflection. This is a notable problem as
many applications of reflection (e.g., hashing and serialization) will require
us to actually access the values in their native forms, which in turn means we
need the types. Unfortunately, Boost::Describe lacks the ability to reflect
constructors, and can not access attributes through getters/setters (needed for
reflecting third party classes). There are open issues on Boost::Describe's
GitHub page for both of these issues, but it's not clear if they are being
worked on at the moment. Ultimately, this means that if we want reflection we
have to roll our own solution.
