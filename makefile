all: packet_scheduling

packet_scheduler: packet_scheduling.c
	gcc packet_scheduling -o packet_scheduling.c

clean:
	rm -f packet_scheduler