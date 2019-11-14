# chess-engine

This is a chess engine I have been developing in my free time.

Currently, it has a working move generation, a simple alpha-beta search and a very basic evaluation function.

For representing the state of the board I use bitboards for every piece and also maintain a square centric lookup table.

I have adopted ideas from the website https://www.chessprogramming.org/Main_Page, but not code.

Move generation is relatively fast, when optimisation flag O3 is turned on, on my machine it gives the following output for positions at https://www.chessprogramming.org/Perft_Results:

`
Position 1
20  ... OKAY
    time: 0.000021ms     nps: 952380.952381
400 ... OKAY
    time: 0.000155ms     nps: 2580645.161290
8902    ... OKAY
    time: 0.001423ms     nps: 6255797.610682
197281  ... OKAY
    time: 0.051997ms     nps: 3794084.274093
`

`
Position 2
48  ... OKAY
    time: 0.000010ms     nps: 4800000.000000
2039    ... OKAY
    time: 0.000467ms     nps: 4366167.023555
97862   ... OKAY
    time: 0.010892ms     nps: 8984759.456482
4085603 ... OKAY
    time: 0.911888ms     nps: 4480378.072746
`

`
Position 3
14  ... OKAY
    time: 0.000004ms     nps: 3500000.000000
191 ... OKAY
    time: 0.000033ms     nps: 5787878.787879
2812    ... OKAY
    time: 0.000398ms     nps: 7065326.633166
43238   ... OKAY
    time: 0.006245ms     nps: 6923618.895116
`

`
Position 4
6   ... OKAY
    time: 0.000007ms     nps: 857142.857143
264 ... OKAY
    time: 0.000064ms     nps: 4125000.000000
9467    ... OKAY
    time: 0.001170ms     nps: 8091452.991453
422333  ... OKAY
    time: 0.094971ms     nps: 4446968.021817
`

`
Position 5
44  ... OKAY
    time: 0.000008ms     nps: 5500000.000000
1486    ... OKAY
    time: 0.000314ms     nps: 4732484.076433
62379   ... OKAY
    time: 0.007709ms     nps: 8091710.987158
2103487 ... OKAY
    time: 0.472868ms     nps: 4448359.795968
`

`
Position 6
46  ... OKAY
    time: 0.000009ms     nps: 5111111.111111
2079    ... OKAY
    time: 0.000370ms     nps: 5618918.918919
89890   ... OKAY
    time: 0.009211ms     nps: 9758983.823689
3894594 ... OKAY
    time: 0.706401ms     nps: 5513290.609725
`
