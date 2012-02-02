all: framework.pdf

framework.pdf: framework.tex flow.pdf makefile
	latexmk --pdf framework

flow.pdf: flow.dot makefile
	dot -Gconcentrate=true -Tpdf flow.dot > flow.pdf

clean:
	latexmk -c framework
	rm -f framework.{nav,snm,vrb}
	rm -f *~

clobber: clean
	latexmk -C framework
