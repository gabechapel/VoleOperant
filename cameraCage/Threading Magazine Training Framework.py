import threading

import queue
import time
import random
import os
import csv


random.seed(4)
#our queues for doign stuff and saving
do_stuff_queue = queue.Queue()
timestamp_queue = queue.Queue()

round_time = 7
timeIII = 2
timeII =2

global interrupt
interrupt = False

global start_time
start_time = time.time()


def extend_levers(q):
    print('extending levers')
    print('LEDs on')
    timestamp_queue.put('Levers out, %f'%(time.time()-start_time))
    q.task_done()

def pellet_tone(q):
    print('starting pellet tone')
    timestamp_queue.put('pellet tone start, %f'%(time.time()-start_time))
    time.sleep(1)
    print('pellet tone complete')
    timestamp_queue.put('pellet tone complete, %f'%(time.time()-start_time))
    q.task_done()

def dispence_pellet(q):
    print('sending dispence pellet signal')
    print('monitoring pellet arrival over serial')
    timestamp_queue.put('pellet out, %f'%(time.time()-start_time))
    q.task_done()
    time.sleep(random.randrange(5,10,1)/2)
    timestamp_queue.put('pellet taken, %f'%(time.time()-start_time))


def retract_levers(q):
    print('LEDs off')
    print('retracting levers')
    timestamp_queue.put('Levers retracted, %f'%(time.time()-start_time))
    q.task_done()

def experiment_start_tone(q):
    print('starting experiment tone')
    timestamp_queue.put('experiment start tone start, %f'%(time.time()-start_time))
    time.sleep(2)
    print('experiment tone complete')
    timestamp_queue.put('experiment start tone start complete, %f'%(time.time()-start_time))
    q.task_done()


def thread_distributor():
    '''this is main thread's job. To look for shit to do and send it to a thread'''
    while True:
        if not do_stuff_queue.empty():
            do = do_stuff_queue.get()

            do(do_stuff_queue)


for x in range(4):

    t = threading.Thread(target = thread_distributor)
    t.daemon = True
    t.start()
    print("started %i"%x )

for i in range(5):
    round_start = time.time()
    print("new round!!!")
    timestamp_queue.put('Starting new round, %f'%(time.time()-start_time))
    do_stuff_queue.put(experiment_start_tone)

    #wait till tone is done
    do_stuff_queue.join()

    do_stuff_queue.put(extend_levers)
    do_stuff_queue.join()

    time.sleep(timeII)

    timeIII_start = time.time()

    #generate fake prob of lever press
    rand = random.randint(1,8)


    while time.time() - timeIII_start < timeIII:

        #eventually, here we will call threads to monitor
        #vole position and the levers. here its just random
        if rand>4 and not interrupt:
            interrupt = True
            print('a lever was pressed! woweeeee')
            timestamp_queue.put('a lever was pressed! woweeeee, %f'%(time.time()-start_time))
            do_stuff_queue.put(pellet_tone)
            do_stuff_queue.put(dispence_pellet)

    do_stuff_queue.put(retract_levers)
    if not interrupt:
        print('the vole is dumb and didnt press a lever')
        do_stuff_queue.put(pellet_tone)
        do_stuff_queue.put(dispence_pellet)


    time.sleep(2)
    while time.time() - round_start < round_time:
        '''just hanging'''
    interrupt = False

os.chdir('/Users/davidprotter/Desktop/Testing Pi stuff/')

with open('output.txt', 'w') as csv_file:
    csv_writer = csv.writer(csv_file)
    while not timestamp_queue.empty():
        csv_writer.writerow(timestamp_queue.get().split(','))
