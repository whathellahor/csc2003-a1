FOR MAPPING:

The map will be represented by nodeArray and isExit. nodeArray is an array of node structs. The node structs contain the x and y coordinates, and the wall openings. isExit is an array storing the node index of the exit node and exit wall.

call the startMapping() method to start mapping.

FOR NAVIGATION:

The nodeArray will be converted into an adjacency matrix to pass into the dijkstra algorithm to generate the shortest path from the start node to the exit/end node.

call the startNavigating() function to start navigating.
