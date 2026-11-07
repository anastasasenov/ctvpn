## Compile-Time Validated Petri Net

[![License: MIT](https://shields.io)](https://opensource.org)
![AI Assisted](https://shields.io)

An experimental end-to-end automated engineering pipeline that transforms standardized
Petri Net Markup Language (PNML) files into strongly typed C++20 template models.
The architecture leverages modern meta-programming capabilities to perform mathematical
model-checking (Liveness and Boundedness proofs) entirely within the compilation phase,
and exports a comprehensive state-space visualization schema in Graphviz DOT format at runtime.

### Architectural Workflow

The complete lifecycle from graphical representation to a validated visual representation
of the reachability space is managed seamlessly across three distinct phases without
passing runtime command-line configurations:

DESIGN ( YAPNE ) -> TRANSFORMATION ( xslt ) -> VERIFICATION ( C++ ) -> EXECUTION ( .dot ) 

{ model.pnml }      { generated_net.hpp }      { static_assert }       { reachability_graph.dot } 

Design Phase: The Petri net model is constructed in a compliant graphical designer and exported 
as a standardized .pnml document containing structural state bounds and configuration weights. 

Transformation Phase: CMake intercepts updates to the model file and automatically fires
the XSLT processor. Alphanumeric network IDs are mapped to flattened sequential matrix indexes,
and initial tokens are injected as a static constexpr std::array.

Compilation & Proof Phase: The C++ compiler executes a custom non-allocating Breadth-First Search
(BFS) graph-crawling algorithm. If an unresolvable structural deadlock is trapped or a token volume
overflows the MaxBound boundary, compilation aborts with a descriptive syntax block.

Execution Phase: The resulting ultra-lean binary is executed. It dumps the calculated
reachability tree to disk, encoding discrete visual color accents across state classifications. 
    
### Build

    $ git clone https://github.com/xxx/ctvpn.git
    $ mkdir build && cd build
    $ cmake ../ctvpn
    $ ./ctvpn
    
    (*) Prerequisites: C++20 and tools

    Debian / Ubuntu

        sudo apt-get update
        sudo apt-get install build-essential xsltproc graphviz xmlproc

    macOS

        brew install xsltproc graphviz

### Step-by-Step

    Step 1: Draw your Petri net using a standard application: model.pnml

    Step 2: Transform pnml xml scheme into C++ structural abstractions
    
        $ xsltproc transform.xslt model.pnml > generated_net.hpp

    Step 3: Compiles with static analysis constraints turned on

        $ g++ -std=c++20 main.cpp -o pn-analyzer

    Step 4: Runs the target engine to yield reachability_graph/tree

        $ ./pn-analyzer


### Inspired by

YAPNE ( https://github.com/chimenkamp/YAPNE-Yet-Another-Petri-Net-Editor )

