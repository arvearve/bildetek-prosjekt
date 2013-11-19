function [result] = ClusterByColor(C, mask, radius, colorDistance, output_folder)
	% Count number of smarties in total.
	cc = bwconncomp(mask);
	% List of each point's location as linear index.
	pp = cc.PixelIdxList; %
	
	
	colors = [];
	rowCol = []; % list of pixels as [row column] vectors
	for i = 1:cc.NumObjects
		% Exctract row/column positions from linear index list in pp.
		[row, col] = ind2sub(size(mask), pp{i}); 
		rowCol = [rowCol; [row col]];
		
		% Average color around this pixel
		sliceR = C( row - radius  : row + radius, col - radius: col + radius, 1);
		sliceG = C( row - radius  : row + radius, col - radius: col + radius, 2);
		sliceB = C( row - radius  : row + radius, col - radius: col + radius, 3);
		avgR = mean(mean(sliceR));
		avgG = mean(mean(sliceG));
		avgB = mean(mean(sliceB));
		
		% Append to our list of candidate colors
		colors = [colors; [avgR avgG avgB]];
	end
	
	% Calculate distances between color values for each point.
	Y = pdist(colors,'euclid'); 
	
	% create binary cluster tree.
	Z = linkage(Y,'single'); 
	
	% Verify consistenct between cluster tree and actual distances.
	% The closer the value of the cophenetic correlation coefficient is to 1,  
	% the more accurately the clustering solution reflects your data.
	
	if cophenet(Z,Y) < 0.9
		'Warning. Possibly bad clustering!\n'
		c = cophenet(Z,Y)
	end
	
	%Prune leafs ( collapse nodes) where distance is < 0.1
	T = cluster(Z,'cutoff', colorDistance, 'criterion', 'distance' ); 
	
	
	% Show awesome diagrams
	colorScatter = figure();
	set(colorScatter,'Visible','off');
	scatter3(colors(:,1), colors(:,2), colors(:,3), 15, colors, 'fill'); 
	saveas( colorScatter, strcat(output_folder, '\7_ColorScatter.png'));
	
	dend = figure();
	set(dend,'Visible','off');
	dendrogram(Z);
	saveas( dend, strcat(output_folder, '\8_Dendrogram.png'));
	
	
	groupScatter = figure();
	set(groupScatter,'Visible','off');
	scatter3(colors(:,1), colors(:,2), colors(:,3), 15, T, 'fill');
	saveas( groupScatter, strcat(output_folder, '\9_GroupScatter.png'));
	
	
	% Create export structure
	%T is a list that maps from point_id to category_id.
	result = [];
	for i = 1:max(T)
		pointIndices = find(T==i);
		count = size(pointIndices, 1);
		color = mean(colors(pointIndices, :), 1);
		points = rowCol(pointIndices, : );
		
		%we have [row, color], but for OpenGL we want [x,y] where 0<={x,y}<=1
		xy = [points(:,2) / cc.ImageSize(2), points(:,1) / cc.ImageSize(1)];  
		thing = struct('color', color, 'count', count, 'rowCol', points, 'xy', xy);
		result = [result; thing];
	end
end
