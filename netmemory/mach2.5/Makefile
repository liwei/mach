all	: checkout master slave stats checkout

master	: master.o
	@-rm master
	cc -o master master.o ../../xp/.@sys/netmemoryUser.o -lmach

slave	: master
	@-rm slave
	ln -s master slave

stats	: master
	@-rm stats
	ln -s master stats

checkout: checkout.o
	cc -o checkout checkout.o -lmach
