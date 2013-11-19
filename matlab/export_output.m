function [] = export_output( result, filename)
	fid = fopen(filename, 'w');
	if fid ~= -1
		fprintf(fid, 'groups: %d\r\n', size(result,1)); 
		fclose(fid); 
	end

	for i=1:size(result)
		fid = fopen(filename, 'a');
		if fid ~= -1
			fprintf(fid, 'color: %f,%f,%f \r\n', result(i).color); 
			fprintf(fid, 'count: %d \r\n', result(i).count);
			fclose(fid); 
		end
		dlmwrite(filename,result(i).xy,'-append', 'newline', 'pc');
	end
end

