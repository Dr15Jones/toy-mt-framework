all: framework.pdf

framework.pdf: framework.tex
	latexmk --pdf framework

clean:
	latexmk -c framework
	rm -f framework.{nav,snm,vrb}
	rm -f *~

clobber: clean
	latexmk -C framework
