NAME=ece
SRC=json.c\
    main.c
CFLAGS=-Wall -O3 -DNDEBUG

$(NAME):
	$(CC) $(SRC) $(CFLAGS) -o $(NAME) -g

clean:
	rm -rf $(NAME)
