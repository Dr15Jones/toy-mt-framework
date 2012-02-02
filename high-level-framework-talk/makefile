all: framework.pdf

framework.pdf: framework.tex flow.pdf
	latexmk --pdf framework

flow.pdf: flow.dot
	dot -Tpdf flow.dot > flow.pdf

clean:
	latexmk -c framework
	rm -f framework.{nav,snm,vrb}
	rm -f *~

clobber: clean
	latexmk -C framework
