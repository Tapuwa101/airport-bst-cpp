# Airport Code Lookup Service (C++)

This project uses a binary search tree (BST) to look up airports by their three-letter IATA code. It was built for the Northeastern University EECE 2560 Airport BST mini project (Fall 2026).

## Files

- `src/airport_bst.cpp`: one C++ file containing the shared Tasks 1–3 code and Tasks 4–6 additions.
- `data/airports.dat`: airport data used by the program.
- `docs/pseudocode.md`: required final combined pseudocode file for Tasks 1–6
- `docs/test-output.txt`: an example test run.

Airport data credit: **OpenFlights.org** (Open Database License).

## Compile and run

Open a terminal inside this folder and enter:

```bash
g++ -std=c++17 -O2 src/*.cpp -o airports
./airports
```

From a different folder, specify the dataset path after the executable.

## Commands

| Command | Purpose |
|---|---|
| `find BOS` | Search and display the path (Task 2) |
| `test2` | Try root, leaf, deep, missing and outside-range search cases (Task 2) |
| `test3` | Test all deletion cases plus a made-up airport on a separate tree (Task 3) |
| `list` | Print all airports in alphabetical IATA-code order and check sorting (Task 4a) |
| `save saved.txt` | Save BST codes in preorder (Task 4b) |
| `load saved.txt` | Rebuild the BST from saved codes; compare height and level order (Task 4b) |
| `level` | Show the full level-order traversal (Task 4b) |
| `validate` | Check the BST range property (Task 4c) |
| `range BOS BWI` | Print matching airports and visited-node count (Task 5) |
| `rebalance` | Rebuild the current tree from the sorted data using middle nodes (Task 6) |
| `stats` | Show height and average depth (Tasks 1 and 6) |
| `experiment` | Measure 10,000 searches in four kinds of BSTs (Task 6) |
| `remove BOS` | Delete an airport and then validate (Task 3) |
| `quit` | Exit |

`list` and `level` can print thousands of codes, so save the output to a file or scroll as needed.

## Sample session

```text
Airports loaded: 6072
Lines skipped: 1626
Duplicates: 0
Height: 177 | Average depth: 18.32
> validate
BST valid: YES
> range BOS BWI
... matching airport records ...
113 airports, 127 nodes visited
> save saved.txt
Saved preorder codes to saved.txt
> load saved.txt
Loaded 6072 airports from saved.txt
Height before: 177, after: 177
Same level order: YES
BST valid: YES
> rebalance
Balanced tree created
Height: 12 | Average depth: 10.65
```

## Task 6 results (example run)

Timing depends on the computer; run `experiment` to get current times.

| Build order | Height | Average depth | 10,000 searches (ms) |
|---|---:|---:|---:|
| File order | 177 | 18.32 | 2.481 |
| Sorted order | 6071 | 3035.50 | 281.050 |
| Balanced | 12 | 10.65 | 1.627 |
| Random shuffle (bonus) | 25 | 14.33 | 2.089 |

File order creates a fairly tall tree because the rows are not shuffled randomly. Sorted insertion creates a chain because every new code goes to the right. The balanced build chooses the middle code first, making the tree much shorter. The random shuffle generally makes a shorter tree than file order.

The BST uses one comparison path from root to a leaf, so operations usually take **O(h)** time, where `h` is tree height. The worst case for the sorted chain is **O(n)**. The balanced tree has height **O(log n)**, so search takes **O(log n)**.
