# C-SAT Solver

## Overview
This project implements a basic SAT Solver in C to solve the Boolean Satisfiability Problem (SAT), one of the most important problems in computer science. The SAT problem involves determining if there is a possible assignment to variables that satisfies a given propositional formula. This solver reads a propositional formula in Reverse Polish Notation (RPN), converts it into Conjunctive Normal Form (CNF) using Tseytin Transformation, and then applies the DPLL (Davis-Putnam-Logemann-Loveland) algorithm to check for satisfiability.

## Features
- Parses propositional formulas in RPN format.
- Converts formulas into CNF using the Tseytin transformation.
- Solves CNF formulas using the DPLL algorithm with optimizations like unit propagation and backtracking.
- Supports testing of various formula inputs using a custom test suite.
 
## Prerequisites
To compile and run this project, you'll need:

- A working C compiler (like gcc).
- Python for running the test scripts.
- make for building the project.
