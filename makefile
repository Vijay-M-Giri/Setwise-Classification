main: main.c helper.c kmeans.c
	gcc -g main.c helper.c kmeans.c R-Tree/card.c R-Tree/index.c R-Tree/node.c R-Tree/rect.c R-Tree/split_l.c -lm
