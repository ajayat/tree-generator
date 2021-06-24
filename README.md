# A beautiful C++ tree generator

It can be printed directly on the output console :
```text
              ┌ Epsilon
              ├─── Zeta
      ┌─ Beta ┤
      │       ├──── Eta
      │       │         ┌───── Mu
      │       └── Theta ┤
      │                 └───── Nu
Alpha ┼ Gamma ────── Xi ─ Omicron
      │       ┌─── Iota
      └ Delta ┼── Kappa
              └─ Lambda
```

You can define relations between nodes on the 
[`edges.json`](https://github.com/Adridri24/TreeGenerator/blob/master/src/json/edges.json)
file.
```json
{
  "Alpha": {
    "Beta": {
      "Epsilon": {},
      "Zeta": {},
      "Eta": {},
      "Theta": {
        "Mu": {},
        "Nu": {}
      }
    },
    "Gamma": {
      "Xi": {
        "Omicron": {}
      }
    },
    "Delta": {
      "Iota": {},
      "Kappa": {},
      "Lambda": {}
    }
  }
}
```

You can also use your own tree representation, the `Tree` class needs only an
*adjacency* table to work:
```cpp
int main() {
    ...
    Tree(adjacences).show(root);
}
```

*Made by Adrien Jayat.*
