

with open('testing.obj') as file:
	point_map = {}
	normal_map = {}
	point_counter = 1
	normal_counter = 1
	
	face_list = []
	point_list = []
	normal_list = []
	for line in file:
		line_list = line.split(' ');
		
		if line_list[0]=='v':
			vector = (float(line_list[1]), float(line_list[2]), float(line_list[3]))
			
			if not vector in point_map:
				point_map[vector] = point_counter
				point_list.append(vector)
				point_counter += 1
		elif line_list[0]=='vn':
			vector = (float(line_list[1]), float(line_list[2]), float(line_list[3]))
			
			if not vector in normal_map:
				normal_map[vector] = normal_counter
				normal_list.append(vector)
				normal_counter += 1
		
		elif line_list[0]=='f':
			point1 = tuple([float(x) for x in line_list[1:4]])
			normal1 = tuple([float(x) for x in line_list[4:7]])
			
			point2 = tuple([float(x) for x in line_list[7:10]])
			normal2 = tuple([float(x) for x in line_list[10:13]])
			
			point3 = tuple([float(x) for x in line_list[13:16]])
			normal3 = tuple([float(x) for x in line_list[16:]])
			
			triangle = (point1, normal1, point2, normal2, point3, normal3);
			
			face_list.append(triangle);
			
with open('pytest.obj', 'w') as new_file:
	
	for pt in point_list:
		new_file.write('v '+str(pt[0])+ " " + str(pt[1])+ " "+  str(pt[2]) + "\n")
	
	for nm in normal_list:
		new_file.write('vn '+str(nm[0])+ " " + str(nm[1])+ " "+  str(nm[2]) + "\n")
	
	for tri in face_list:
		new_file.write('f '+str(point_map[tri[0]])+'//'+str(normal_map[tri[1]])+' '+str(point_map[tri[2]])+'//'+str(normal_map[tri[3]])+' '+
						str(point_map[tri[4]])+'//'+str(normal_map[tri[5]])+'\n')
	
	
	
	
	
	
	
	
	
	
	
			