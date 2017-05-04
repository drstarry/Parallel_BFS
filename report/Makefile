all:
	@pdflatex project.tex
	@bibtex project.aux
	@pdflatex project.tex
	@pdflatex project.tex

quiet:
	@pdflatex project.tex &>/dev/null
	@bibtex project.aux &>/dev/null
	@pdflatex project.tex &>/dev/null
	@pdflatex project.tex &>/dev/null

clean:
	@rm -f project.aux project.bbl project.blg project.log project.out project.pdf references.log project.synctex.gz
