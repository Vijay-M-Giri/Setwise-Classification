import random
import matplotlib.pyplot as plt
import math

def rotate_point(A, B, ang):
	x = B[0] + (A[0]-B[0])*math.cos(ang) - (A[1]-B[1])*math.sin(ang)
	y = B[1] + (A[0]-B[0])*math.sin(ang) + (A[1]-B[1])*math.cos(ang)
	return (x,y)

#finds the area of the triangle given its 3 coordinates
def area_triangle(a, b, c):
	area = abs(a[0]*(b[1]-c[1])+b[0]*(c[1]-a[1])+c[0]*(a[1]-b[1]))/2;
	return area

def point_on_triangle(pt1, pt2, pt3):
	"""
	Random point on the triangle with vertices pt1, pt2 and pt3.
	"""
	s, t = sorted([random.random(), random.random()])
	return (s * pt1[0] + (t-s)*pt2[0] + (1-t)*pt3[0], s * pt1[1] + (t-s)*pt2[1] + (1-t)*pt3[1])

def point_on_rect(left_point, length, width, ang):
	"""
	Random point on the rectangle with lowermost vertex left_point, length, width and ang = angle of rotation of the rectangle about left_point
	"""
	x = random.uniform(left_point[0], left_point[0]+length)
	y = random.uniform(left_point[1], left_point[1]+width)
	return rotate_point((x,y), left_point, ang)

def point_on_ellipse(centre, width, height, ang):
	# ang is the angle about which the generated point has to be rotated about the centre of the ellipse
	rad = random.uniform(0.0, 1.0)
	phi = random.uniform(0.0, 2*math.pi);
	x = math.sqrt(rad) * math.cos(phi)
	y = math.sqrt(rad) * math.sin(phi)
	x *= width/2;
	x += centre[0]
	y *= height/2; 
	y += centre[1]
	return rotate_point((x,y), centre, ang)

def pointlist_on_pgram(p1, p2, p3, p4):
	# consider a parallelogram as a conjunct of 2 triangles
	area1 = area_triangle(p1, p2, p3)
	area2 = area_triangle(p1,p3,p4)
	num1 = int(area1/(area1+area2) * 10000)
	num2 = 10000 - num1
	points_pgram = [point_on_triangle(p1,p2,p3) for _ in range(num1)] + [point_on_triangle(p1,p3,p4) for _ in range(num2)]
	return points_pgram

# takes as input an array of points and returns a list of points lying uniformly inside the polygon by splitting the polygon into triangles
def pointlist_on_polygon(parr, ang):
	num_points = []		# list of points in the ith rectangle
	ar = []
	sum_area = 0
	poly_points = []
	for i in range(1, len(parr)-1):
		ar.append(area_triangle(parr[0], parr[i], parr[i+1]))
		sum_area += ar[-1]
	for area in ar:
		num_points.append(int(area*1.0/sum_area * 10000))
	print (num_points)
	for i in range(len(num_points)):
		poly_points += [rotate_point(point_on_triangle(parr[0],parr[i+1],parr[i+2]), parr[0], ang) for _ in range(num_points[i])]
	return poly_points

def main():
	pt1 = (1, 1)
	pt2 = (2, 4)
	pt3 = (5, 2)
	points_tri = [point_on_triangle(pt1, pt2, pt3) for _ in range(10000)]
	points_tri += [point_on_triangle((-5,0),(-3.52,1.37),(-1,0)) for _ in range(10000)]
	points_tri += [point_on_triangle((-8.5,5),(-6,5),(-6,7)) for _ in range(10000)]
	points_tri += [point_on_triangle((5,16),(6,12.5),(8,12.5)) for _ in range(10000)]
	points_tri += [point_on_triangle((-4,9.5),(-3,10.75),(-2,9.5)) for _ in range(10000)]

	points_rect = [point_on_rect((18,0), 0.3, 10, 0) for _ in range(10000)]
	points_rect += [point_on_rect((-5,5), 4, 0.4, 0) for _ in range(10000)]
	points_rect += [point_on_rect((-10,0), 4, 4, 0) for _ in range(10000)]
	points_rect += [point_on_rect((-10.5,8.5), 3, 1.5, -math.pi/4) for _ in range(10000)]
	points_rect += [point_on_rect((5.5,1), 3, 2, 0) for _ in range(10000)]

	points_ellp = [point_on_ellipse((-3,3), 2, 4, math.pi/4) for _ in range(10000)]
	points_ellp += [point_on_ellipse((-10,6), 2, 2, 0) for _ in range(10000)]
	points_ellp += [point_on_ellipse((2.2,5.75), 2.75 ,1.5, 0) for _ in range(10000)]
	points_ellp += [point_on_ellipse((10.5,14), 1.5 ,4, -math.pi/6) for _ in range(10000)]
	points_ellp += [point_on_ellipse((-1,14), 3, 3, 0) for _ in range(10000)]

	#for parallelogram
	#p1 = (-2,-2); p2 = (-4,-3.5); p3 = (-3,5); p4 = (-1,4)
	p1 = (4.75,5.00); p2 = (8,6); p3 = (7.2, 8); p4 = (4,7)
	points_pargram = pointlist_on_pgram(p1, p2, p3, p4)
	points_pargram += pointlist_on_pgram((-5,6.5),(-4,8.5),(-1,8.5),(-2,6.5))
	points_pargram += pointlist_on_pgram((-8,10),(-6,12),(-3,12),(-5,10))
	points_pargram += pointlist_on_pgram((12,4),(11,6),(13,6),(14,4))
	points_pargram += pointlist_on_pgram((9,9),(10.5,10.5),(12,9),(10.5,7.5))

	parr = [(12,14), (14,16), (16,16), (18,14), (16,12), (14,12)]
	poly_points = pointlist_on_polygon(parr, 0)
	parr = [(1,8),(2,9),(3,9),(4,8),(3,7),(2,7)]
	poly_points += pointlist_on_polygon(parr, math.pi/4)
	parr = [(1,14),(2,16),(3,16),(4,14),(3,12),(2,12)]
	poly_points += pointlist_on_polygon(parr, 0)
	parr = [(-8.5,15),(-7.5,16),(-6.5,16),(-5.5,15),(-6.5,14),(-7.5,14)]
	poly_points += pointlist_on_polygon(parr, -math.pi/4)
	parr = [(10,2),(12,3),(14,3),(16,2),(14,1),(12,1)]
	poly_points += pointlist_on_polygon(parr, 0)


	pt1 = (1+30, 1)
	pt2 = (2+30, 4)
	pt3 = (5+30, 2)
	points_tri += [point_on_triangle(pt1, pt2, pt3) for _ in range(10000)]
	points_tri += [point_on_triangle((-5+30,0),(-3.52+30,1.37),(-1+30,0)) for _ in range(10000)]
	points_tri += [point_on_triangle((-8.5+30,5),(-6+30,5),(-6+30,7)) for _ in range(10000)]
	points_tri += [point_on_triangle((5+30,16),(6+30,12.5),(8+30,12.5)) for _ in range(10000)]
	points_tri += [point_on_triangle((-4+30,9.5),(-3+30,10.75),(-2+30,9.5)) for _ in range(10000)]

	points_rect += [point_on_rect((18+30,0), 0.3, 10, 0) for _ in range(10000)]
	points_rect += [point_on_rect((-5+30,5), 4, 0.4, 0) for _ in range(10000)]
	points_rect += [point_on_rect((-10+30,0), 4, 4, 0) for _ in range(10000)]
	points_rect += [point_on_rect((-10.5+30,8.5), 3, 1.5, -math.pi/4) for _ in range(10000)]
	points_rect += [point_on_rect((5.5+30,1), 3, 2, 0) for _ in range(10000)]

	points_ellp += [point_on_ellipse((-3+30,3), 2, 4, math.pi/4) for _ in range(10000)]
	points_ellp += [point_on_ellipse((-10+30,6), 2, 2, 0) for _ in range(10000)]
	points_ellp += [point_on_ellipse((2.2+30,5.75), 2.75 ,1.5, 0) for _ in range(10000)]
	points_ellp += [point_on_ellipse((10.5+30,14), 1.5 ,4, -math.pi/6) for _ in range(10000)]
	points_ellp += [point_on_ellipse((-1+30,14), 3, 3, 0) for _ in range(10000)]

	#for parallelogram
	#p1 = (-2,-2); p2 = (-4,-3.5); p3 = (-3,5); p4 = (-1,4)
	p1 = (4.75+30,5.00); p2 = (8+30,6); p3 = (7.2+30, 8); p4 = (4+30,7)
	points_pargram += pointlist_on_pgram(p1, p2, p3, p4)
	points_pargram += pointlist_on_pgram((-5+30,6.5),(-4+30,8.5),(-1+30,8.5),(-2+30,6.5))
	points_pargram += pointlist_on_pgram((-8+30,10),(-6+30,12),(-3+30,12),(-5+30,10))
	points_pargram += pointlist_on_pgram((12+30,4),(11+30,6),(13+30,6),(14+30,4))
	points_pargram += pointlist_on_pgram((9+30,9),(10.5+30,10.5),(12+30,9),(10.5+30,7.5))

	parr = [(12+30,14), (14+30,16), (16+30,16), (18+30,14), (16+30,12), (14+30,12)]
	poly_points += pointlist_on_polygon(parr, 0)
	parr = [(1+30,8),(2+30,9),(3+30,9),(4+30,8),(3+30,7),(2+30,7)]
	poly_points += pointlist_on_polygon(parr, math.pi/4)
	parr = [(1+30,14),(2+30,16),(3+30,16),(4+30,14),(3+30,12),(2+30,12)]
	poly_points += pointlist_on_polygon(parr, 0)
	parr = [(-8.5+30,15),(-7.5+30,16),(-6.5+30,16),(-5.5+30,15),(-6.5+30,14),(-7.5+30,14)]
	poly_points += pointlist_on_polygon(parr, -math.pi/4)
	parr = [(10+30,2),(12+30,3),(14+30,3),(16+30,2),(14+30,1),(12+30,1)]
	poly_points += pointlist_on_polygon(parr, 0)



	points = points_tri + points_rect + points_ellp + points_pargram + poly_points

	with open("shapes_points.txt", 'w') as fp:
		for i in range(len(points_tri)):
			pt = points_tri[i]
			clab = 1
			enid = i//10000 + 1;
			fp.write(str(pt[0]) + " " + str(pt[1]) + " " + str(enid) + " " + str(clab) + "\n")
		for i in range(len(points_rect)):
			pt = points_rect[i]
			clab = 2
			enid = 11 + i//10000
			fp.write(str(pt[0]) + " " + str(pt[1]) + " " + str(enid) + " " + str(clab) + "\n")
		for i in range(len(points_ellp)):
			pt = points_ellp[i]
			clab = 3
			enid = 21 + i//10000
			fp.write(str(pt[0]) + " " + str(pt[1]) + " " + str(enid) + " " + str(clab) + "\n")
		for i in range(len(points_pargram)):
			pt = points_pargram[i]
			clab = 4
			enid = 31 + i//10000
			fp.write(str(pt[0]) + " " + str(pt[1]) + " " + str(enid) + " " + str(clab) + "\n")
		for i in range(len(poly_points)):
			pt = poly_points[i]
			clab = 5
			enid = 41 + i//10000
			fp.write(str(pt[0]) + " " + str(pt[1]) + " " + str(enid) + " " + str(clab) + "\n")


	x, y = zip(*points)
	# plt.xlim(-2, 10)
	# plt.ylim(-2, 10)
	plt.scatter(x, y, s=0.1)
	#plt.savefig('shapes1.png', bbox_inches='tight')
	plt.show()
	
main()