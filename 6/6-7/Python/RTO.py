import os

if __name__ == '__main__':
    RTTArray = [26, 32, 24, 26, 26, 28, 26, 26, 28, 26, 26, 28, 30]
    Alpha = 0.125
    Beita = 0.25
    RTT = RTTArray[0]
    RTTs = RTT
    RTTd = RTT / 2
    RTO = RTTs + 4 * RTTd
    print("Initial RTT: {}".format(RTT))

    for i in range(1, len(RTTArray)):
        RTT = RTTArray[i]
        RTTs = (1 - Alpha) * RTTs + Alpha * RTT
        RTTd = (1 - Beita) * RTTd + Beita * abs(RTTs - RTT)
        RTO = RTTs + 4 * RTTd
        print("Round: {}, RTT: {}, RTTs: {}, RTO: {}".format(i, RTT, RTTs, RTO))
    os.system("pause")

