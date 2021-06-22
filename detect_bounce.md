1. Collision with the left and right borders and bounce boards 

right border: If the x-coordinate of the ball plus the width of the ball is greater than the size of the window, it will collide and bounce.

left border: If the x-coordinate of the ball is less than 0, it will collide and bounce.

top bounce board: if the y-coordinate of the ball is greater than the height of the board, it will collide and bounce.

bottom bounce board: if the y-coordinate plus the height of the ball is less than the board y-coordinate, it will collide and bounce.
 

2. Collision with blocks
 First, we will let the blocks determine the position of the ball through the x y coordinates. For example, if the x coordinate of the ball is smaller than the x coordinate of the blocks, then the ball is on the left side of the blocks.
Judging the position
The square will record the contact edge with the ball, and calculate the distance to the ball from the contact edge. If the distance is less than the radius of the ball, it will be judged as a collision.

3. Direction of the bounce
We calculate the direction of the bounce, which is judged from the part of "ball trajectory control".
Based on int check_collision()and double GetDistance(int x1, int y1, int x2, int y2) calculations.There are the coordinates of the ball and the radius of the circle.When the distance between two points is less than the radius, a collision occurs.
Taking the right boundary of the ball bounced as an example, when the coordinates of the ball are greater than or equal to the width of the window, it will bounce to the left.


