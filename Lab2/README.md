###### tags:`PDA`
# PDA HW2
## Execution
```shell
make
./LAB2 [input]
```
The default output file name is `output.txt`. Or you can specify an output filename:
```shell
./LAB2 [input] [output]
```

## Definition
* F(i): number of nets that have cell *i* as the only cell in `From Block`
* T(i): number of nets that contain cell *i* and are entirely located in `From Block`
* gain(i) = F(i) - T(i)

Pseudo code:
```c=
G = 1
set an initial partition
calculate all gains
while G > 0
    for i = 1 to n do
        choose a unlock cell V with largest gain Dv and moving V won't violate the area constraint
        move V and lock V
        let gi = Dv
        update the gains of cells that connected to V
    find the k s.t. G = g1 + g2 + ... + gk is maximized and having the best balance
    move the first k vertices and record this solution
    unlock all vertices
```

```c=
Algorithm: Update_Gain
begin /* move base cell and update neighbors' gains */
F ← the From Block of the base cell;
T ← the To Block of the base cell;
Lock the base cell and complement its block;
for each net n on the base cell do
    /* check critical nets before the move*/
    if T(n) = 0 then increment gains of all free cells on n (case 4)
    else if T(n) = 1 then decrement gain of the only T cell on n, if it is free (case 1,2)

    /*change F(n) and T(n) to reflect the move*/
    F(n) ← F(n) - 1;
    T(n) ← T(n) + 1;

    /*check for critical nets after the move*/
    if F(n) = 0 then decrement gains of all free cells on n(case 1)
    else if F(n) = 1 then increment gain of the only F cell on n, if it is free (case 3,4)
end
```
* case 1:
    ![](https://i.imgur.com/Q1zTS78.png)
* case 2:
    ![](https://i.imgur.com/jhePqOf.png)
* case 3:
    ![](https://i.imgur.com/xoKBU2w.png)
* case 4:
    ![](https://i.imgur.com/GeebQJN.png)