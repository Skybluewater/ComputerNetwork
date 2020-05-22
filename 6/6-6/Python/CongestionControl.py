import os

if __name__ == '__main__':
    MSS = 1024
    Threshold = 32768
    TriAckRound = 16
    TimeoutRound = 22
    EndRound = 26

    cwnd = 1
    ssthresh = int(Threshold / MSS)
    print("MSS: {}, Initial Threshold: {}".format(MSS, Threshold))
    print("For conveience, change unit from byte to segment, so initial threshold is {} segments".format(ssthresh))
    for i in range(1, EndRound + 1):
        print("Round: {}, cwnd: {}, send data".format(i, cwnd))
        if i == TriAckRound:
            ssthresh = int(cwnd / 2)
            cwnd = ssthresh
        elif i == TimeoutRound:
            ssthresh = int(cwnd / 2)
            cwnd = 1
        else:
            if cwnd < ssthresh:
                cwnd *= 2
            else:
                cwnd += 1
    os.system("pause")
