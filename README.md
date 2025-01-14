<div align="center">
  <!--   ![Logo for hypercomm](logo.png) -->
  <img src="logo.png" width="320">

  </br>

  [![SMP](https://github.com/jszaday/hypercomm/actions/workflows/smp.yml/badge.svg)](https://github.com/jszaday/hypercomm/actions/workflows/smp.yml)
  [![Non-SMP](https://github.com/jszaday/hypercomm/actions/workflows/non_smp.yml/badge.svg)](https://github.com/jszaday/hypercomm/actions/workflows/non_smp.yml)
  [![Total alerts](https://img.shields.io/lgtm/alerts/g/jszaday/hypercomm.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jszaday/hypercomm/alerts/)
  [![Language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/jszaday/hypercomm.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/jszaday/hypercomm/context:cpp)

</div>


# A Foreword Regarding Status

Hypercomm is a collection of libraries that aim to enhance various high-level abstractions in Charm++, both in terms of performance and generality (i.e., raising the abstraction level). Its components-centric model provides lightweight sections that do not require registration. Components form the basis of an abstraction for parallelism, with each having a common set of operations for producing and consuming values and dictating their lifecycle. This abstraction incorporates ideas and terms from various sources, like distributed tuple-spaces and SPECTRE/HPX (shared notions of components/actions), and applies them to Charm++, recasting some of its core features along the way.

Its goal is, primarily, to underpin high-level languages, like Ergoline. Its approach grants compilers a great deal of flexibility with respect to abstraction construction; however, direct users of its current API may notice its verbosity relative to a fully compiler-integrated solution. Subsequently, long-term goals include integrating with Charmxi or exploiting TMP to reduce verbosity (with/a la SPECTRE).

In any case, its modules were developed as components of three projects. Some features still need to be integrated from their respective sources; the breakdown of the remaining work follows here:

- Hypercomm Aggregation:

  - Sections/Reductions – Replaced.
  - Aggregators – Pending, to be reimplemented as components.
- Ergoline:
  - De/serialization (a.k.a. _ser//des_) – Nearly Integrated, rename ergoline::array to hypercomm::span.
  - Hashing – Integrated.
  - Mailboxes – Pending, (recently) reworked components to support requests.
  - Futures – Integrated (without is_ready).
  - Callbacks &amp; Reductions – Replaced.
  - Ergoline is still on an older, now historical branch of this library. Once this integration is complete, it needs to be updated.
- Hypercomm Components:
  - Components – Integrated, uses _ser//des_.
  - Vils, Sections, Multicasts and Reductions – Integrated.

Other work remaining before a release will be considered:

- Correct QD-Component Interaction.
- Add a simplified connection API?
- Use process-awareness to provide node-invariant reduction/multicast performance.
- Add range-based and global lightweight sections (driving optimizations)
- Enable using vils with groups and nodegroups (low difficulty)
- Enforce a uniform module naming and layout scheme (refactoring)
- Enable de/serialization of components to facilitate migratability (low difficulty)
- Add &quot;Refresh&quot; for Rebuilding Sections:
  - Support dynamism like chare-migration and array insertion/deletion (high difficulty)
  - Allow wrapping conventional Charm++ sections as Hypercomm sections (low difficulty)
- Add Support for Cross-Collective Sections (moderate difficulty)
- Directly Offer a Counterpart for Entry Methods (low difficulty)
- Enable Using a const ref& to an std::vector as a section.

## Regarding Hypercomm's Model

Components are fine-grained, polymorphic objects that, effectively, encapsulate an action. Components have a common API that can represent and implement higher-level constructs, like futures, channels, mailboxes, reductions, multicasts, and tasks. Chares are recast as virtual localities, or _vils_, with entry ports that forward messages to their components&#39; input ports; these are, otherwise, encapsulated and remotely inaccessible.

Tying an entry port to a persistent action corresponds to the entry methods of chares; however, entry ports have a high degree of flexibility and dynamism. Entry ports may be added and removed throughout a vils&#39; lifecycle; note, they have a lifecycle of their own that may be tied to individual/one-time operations. Reducers are implemented as components that are created whenever a local contribution occurs. They open designated entry ports for each reduction, assigning them a unique ID based on a sequential counter for the _SOCK_ (_Section, Operation, CallbacK_) triplet. Each reducer combines any received values via its ports with its local contribution, then forwards the result to another reducer or the callback.

Virtual localities may also be sent _tasks_ that directly encapsulate actions. For example, a multicast may be implemented as a broadcaster task that delivers a message locally, then copies itself to its &quot;children&quot;; this information is derived from a section&#39;s spanning tree.

Virtual localities may be as chares are to chare collectives, forming groups, nodegroups, or chare-arrays with generic indices.

## Building Hypercomm
Hypercomm utilizes the Cmake build system. Ensure that the environment variable `CHARM_HOME` is set, run cmake in the top-level directory (i.e., `cmake .`), then run the generated makefiles (i.e., `make -j`). The examples use a mixture of conventional makefiles and CMake, these exemplify how Hypercomm can be used with existing applications.
