all:
	cd client && make
	cd server && make

clean:
	rm client/client server/server
