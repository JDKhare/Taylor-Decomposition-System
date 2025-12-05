# Taylor Decomposition System (TDS)

Taylor Decomposition System (TDS) is a research framework for transforming and optimizing arithmetic data paths using Taylor Expansion Diagrams (TEDs). It operates at the behavioral level, generates and manipulates Data Flow Graphs (DFGs), and interfaces with high-level synthesis tools such as GAUT.

TDS supports variable reordering, decomposition, common-subexpression elimination, retiming, and bit-width/accuracy analysis, enabling design-space exploration before synthesis to hardware.

---

## Table of Contents

1. Overview
2. Key Features
3. System Requirements
4. Installation and Setup
5. Running TDS
6. Basic Examples
7. Environment and Configuration
8. Project Structure
9. Documentation
10. Citation
11. License

---

## Overview

TDS is based on Taylor Expansion Diagrams (TEDs), a canonical representation for polynomial and algebraic data paths. Using TEDs, TDS can:

- Capture the functionality of arithmetic expressions at a high level.
- Transform and optimize Data Flow Graphs for multiple cost metrics (latency, number of multipliers/adders, mux count, etc.).
- Interface with GAUT (CDFG) and netlist formats for downstream synthesis and implementation.

---

## Key Features

Major capabilities include:

### TED Construction
- `poly` to build TED from algebraic expressions.
- `dfg2ted`, `ntl2ted` to convert from DFG/NTL.

### DFG and Netlist Generation
- `ted2dfg` to generate structural DFG.
- `dfg2ntl`, `ntl2ted` for round-trip integration with GAUT.

### Optimization and Reordering
- `reorder`, `reorder*`, `sift`, `flip`, `reloc`, `balance`, `dfgarea`.
- Cost analysis using `cost`.

### Common Subexpression Elimination
- `scse`, `dcse`, `candidate`.

### Bit-width and Error Analysis
- `set` and `compute` for bit-widths, ranges, and error bounds.
- Gappa integration for formal accuracy metrics.

### Functional Retiming
- TED-level retiming via `retime`.

---

## System Requirements

Recommended environment:

- Linux Mint / Ubuntu (64-bit)
- GCC or Clang toolchain
- `make`
- Libraries: `libreadline-dev`, `libncurses-dev`
- Optional: `graphviz`, `evince` for graph visualization

TDS builds and runs in both native Linux and VirtualBox Linux VMs.

---

## Installation and Setup

See the accompanying `SETUP_GUIDE.md` for detailed instructions, including:

- VirtualBox workflow
- Linux Mint installation
- Required package installation
- Building TDS from source
- Running the shell

---

## Running TDS

Start the tool via:

```bash
./tds
```

You should see:

```bash
Tds 01>
```

Project Structure

```bash
.
├── src/                     Core implementation
├── docs/                    Manuals, papers, slides
├── examples/                Benchmark polynomial files
├── tds.env                  Environment configuration
├── tds.aliases              Alias definitions
├── Makefile
└── tds                      Executable
```

Documentation:

Included documents:

```bash
TDS Manuals (2011, 2013)

TDS Commands Summary

TDS Presentation Slides
```

Citation:

If you use TDS in academic research, please cite the associated publications (TED/DFG transformation papers and manuals).

