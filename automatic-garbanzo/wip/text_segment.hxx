#ifndef TEXT_SEGMENT_HPP
#define TEXT_SEGMENT_HPP

//a segment of text. has no ownership of the chars it points to
struct Text_Segment
{
	//pointers to the start and end of the segment
	const char* start;
	const char* end;


	bool operator== (const char* c_string) const
	{
		size_t i;
		for(i = 0; &start[i] <= end; i++)
		{
			if( start[i] != c_string[i] ) return false;
			if( c_string[i] == '\0' ) return false;
		}
		if(( &start[i] > end )&&( c_string[i] == '\0')) return true;
		else return false;
	}

	bool operator== (const Text_Segment& other) const;



	//Subsegment functions
	const char* gotoChar (char c, const char* from = nullptr) const
	{
		if(from < start) from = start;
		while(*from != c && from < end)
		{
			from++;
		}
		return from;
	}
	
	bool isWhiteSpace (const char* c) const
	{
		if( end < c || c < start ) return true;//pointer outside of (start,end) is also whitespace
		const char whitespace_chars[] = {' ','\t','\n'};//these chars are whitespace
		for(auto x : whitespace_chars)
			{
				if (*c == x) return true;
			}
		return false;
	}

	//returns pointer to the next non-whitespace char
	const char* skip_whitespace (const char* from) const
	{
		//if(from < start) from = start;
		while(isWhiteSpace(from) && from < end) from++;
		return from;
	}

	const char* end_of_word (const char* from) const
	{
		//if(from < start) from = start;
		while(!isWhiteSpace(from+1)) from++;
		return from;
	}

	const char* next_word (const char* from) const
	{
		if(!isWhiteSpace(from)) from = end_of_word(from)+1;//next whitespace
		from = skip_whitespace(from);//beginning of next word
		return from;
	}

	//returns nth word as a text segment
	Text_Segment word (size_t n) const
	{
		const char* word_start = skip_whitespace(start);//start at non-whitespace
		for(size_t i = 0; i < n; i++) word_start = next_word(word_start);//skip n words

		const char* word_end = end_of_word(word_start);
		Text_Segment word = {word_start, word_end};
		return word;
	}




	//Parsing functions
	//todo - allow explicit casting
	float parseAsFloat () const
	{
		//this will break very easily - make tocstr() or limited atof
		return atof(start);//todo
	}

	int parseAsInt () const
	{
		//this will break very easily - make tocstr() or limited atoi
		return atoi(start);//todo
	}

	void print() const
	{
		for(const char* i = start; i <= end; i++)
		{
			std::cout << *i;
		}
	}
};

struct Text_Blob
{
	//members
	size_t blob_size = 0;
	char* blob = nullptr;

	size_t num_newlines = 0;
	char** newline_indexes = nullptr;//index vs raw pointer?

	~Text_Blob() { delete[] blob; };


	//methods
	size_t count_char(const char c)
	{
		size_t count = 0;
		for(size_t i = 0; i < blob_size; i++)
			if(blob[i] == c) count++;
		return count;
	}


	void getNewlineIndexes()
	{
		num_newlines = count_char('\n');
		delete[] newline_indexes;
		newline_indexes = new char*[num_newlines];
		size_t j = 0;
		for(size_t i = 0; i < blob_size; i++)
		if(blob[i] == '\n')
		{
			newline_indexes[j] = &blob[i];
			j++;
		}
	}

	const char* getLineStart(size_t index)
	{
		if(index == 0) return blob;
		else return newline_indexes[index-1]+1;
	}

	const char* getLineEnd(size_t index)
	{//todo
		return newline_indexes[index];
	}

	template <class T>//todo - currently assumes that file ends on newline
	void forEachLine(T func)//func takes a const_char* to the first and last chars it should operate on.
	{
		getNewlineIndexes();
		for(size_t i = 0; i < num_newlines; i++)
		{
			const char* line_start = getLineStart(i);
			const char* line_end = getLineEnd(i);
			func( line_start, line_end );
		}
	}

	void slurp_file(const char* filename)
	{
		std::ifstream file(filename, std::ios::binary | std::ios::ate);

		if (!file.is_open()) {//tohttps://www.youtube.com/watch?v=ZSdYH3zDLm8do
			std::cout << "failed to open file!" << std::endl;
			exit(420);
		}

		blob_size = static_cast<size_t>( file.tellg() );
		if(blob) delete[] blob;
		blob = new char[blob_size];
		
		file.seekg(0);
		file.read(blob, static_cast<std::streamsize>( blob_size ));
		file.close();
	}


};


#endif
