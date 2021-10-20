###### tags: PDA
# PDA HW1
## Execution
```shell
make
./main [input]
```
The default output file name is `output.txt`. Or you can specify a output filename:
```shell
./main [input] [output]
```

## Function Description

* ***Point Finding***
Given a 'hint' tile(can be ignored) to find the tile at a given (x,y) location.
    1. First move up or down, using right top (**rt**) and left bottom (**lb**) stitches, until a tile is found whose **vertical range contains the desired point**.
    2. Then move left or right, using tr and lb stitches, until a tile is found whose horizontal range contains the desired point.
    3. Since the horizontal motion may have introduced a vertical misalignment, steps 1. and 2. may have to be iterated several times to locate the tile containing the point. The convexity of the tiles guarantees that the algorithm will converge.

* ***Neighbor Finding***
Find all the tiles(space or block) that touch one side of a given tile. The following algorithm shows how to find all the tiles at the right side. Similiar algorithms can be devised to search each of the other sides.
    1. Follow the tr stitch of the starting tile to find its topmost right neighbor.
    2. Then trace down through lb stitches until all the neighbors have been found (the last neighbor is the first tile encountered whose lower y coordinate is less than or equal to the lower y coordinate of the starting tile).
    
* ***Area Searches***(may not need)
To see if there are any solid tiles within a given area.
    1. Use the point-finding algorithm to locate the tile containing the **upper left** corner of the area of interest.
    2. See if the tile is solid. If not, it must be a space tile. See if its right edge is within the area of interest. If so, it is the edge of a solid tile.
    3. If a solid tile was found in step 2, then the search is complete. If no solid tile was found, then move down to the next tile touching the right edge of the area of interest. This can be done either by invoking the point-finding algorithm, or by traversing the lb stitch down and then traversing tr stitches right until the desired tile is found.
    4. Repeat steps 2 and 3 until either a solid tile is found or
the bottom of the area of interest is reached.

* ***Tile Creation***
If there are no existing solid tiles in the desired area of the new tile, insert the tile into the data structure, clip and merge space tiles and updating corner stitches.
    1. Find the space tile containing the top edge of the area to be occupied by the new tile (because of the strip property, a single space tile must contain the entire edge).
    2. Split the top space tile along a horizontal line into a piece entirely above the new tile and a piece overlapping the new tile. Update corner stitches in the tiles adjoining the new tile.
    3. Find the space tile containing the bottom edge of the new solid tile, split it in the same fashion, and update stitches around it.
    4. Work down along the left side of the area of the new tile, as for the area-search algorithm. Each tile along this edge must be a space tile that spans the entire width of the new solid tile. Split the space tile into a piece entirely to the left of the new tile, a piece entirely to the right of the new tile, and a piece entirely within the new tile. This splitting may make it possible to merge the left and right remainders vertically with the tiles just above them: merge whenever possible. Finally, merge the center space tile with the solid tile that is forming. Each split or merge requires stitches to be updated in adjoining tiles.



## Reference 
* [J. K. Ousterhout, "Corner Stitching: A Data-Structuring Technique for VLSI Layout Tools," in IEEE Transactions on Computer-Aided Design of Integrated Circuits and Systems, vol. 3, no. 1, pp. 87-100, January 1984, doi: 10.1109/TCAD.1984.1270061.](https://ieeexplore.ieee.org/abstract/document/1270061)
