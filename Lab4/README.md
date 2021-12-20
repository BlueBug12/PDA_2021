###### tags:`PDA`
# PDA HW4
## Execution
```shell
make
./LAB4 [input.aux]
```
testcase: newblue5
![](https://i.imgur.com/9RiXigG.png)
zoom in:
![](https://i.imgur.com/IkLbeIz.png)

testcase: adaptec1
![](https://i.imgur.com/8Bg9MZu.png)
zoom in:
![](https://i.imgur.com/dGkcGmB.png)



Problem:
1. How to store the original cluster and recover after each try? => store the cluster vector in row
2. How to know how many space each row remains to place a new cell? => don't need to know. If fail, just recover the original vector
3. How to compute cost? =>　the displacement of placed cell()
4. How to choose row? => use binary serach to find the closest row(in height)
5. Where to store nodes? => in row

