all:
	#generate postscript files
	docbook2ps user.dbk
	docbook2ps documentation.dbk
	docbook2ps development.dbk

	#generate pdf file (probably overkill)
	docbook2pdf user.dbk
	docbook2pdf documentation.dbk
	docbook2pdf development.dbk

	#generate html files
	#each book needs it own directory 
	# or there will be a naming conflict 
	mkdir user
	docbook2html -o user user.dbk

	mkdir documentation
	docbook2html -o documentation documentation.dbk

	mkdir development
	docbook2html -o development development.dbk

clean:
	#remove html files
	rm -rf user
	rm -rf documentation
	rm -rf development
	
	#remove all other generated files
	rm *.ps
	rm *.pdf
	rm *.out
