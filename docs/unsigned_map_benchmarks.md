## unsigned_map Benchmarks

```
########################################################
Benchmark using linear keys, 0 to 2500000, no duplicates
########################################################
Num unique keys : 2500000

###############################
Copy ctor 2500000 small objects
###############################
std::unordered_map copy ctor                                  1.151719s        1.000000x
std::map copy ctor                                            0.792965s        1.452421x
fea::unsigned_map copy ctor                                   0.032632s        35.293835x

#############################
Copy ctor 2500000 big objects
#############################
std::map copy ctor                                            4.309895s        1.000000x
std::unordered_map copy ctor                                  2.918783s        1.476607x
fea::unsigned_map copy ctor                                   1.190347s        3.620705x

###########################
Clear 2500000 small objects
###########################
std::unordered_map clear                                      0.537538s        1.000000x
std::map clear                                                0.335137s        1.603935x
fea::unsigned_map clear                                       0.000000s        infx

#########################
Clear 2500000 big objects
#########################
std::map clear                                                2.219605s        1.000000x
std::unordered_map clear                                      1.756794s        1.263441x
fea::unsigned_map clear                                       0.000000s        infx

############################
Insert 2500000 small objects
############################
std::unordered_map insert                                     0.378286s        1.000000x
std::map insert                                               0.346830s        1.090697x
fea::unsigned_map insert                                      0.028351s        13.342763x

##########################
Insert 2500000 big objects
##########################
std::map insert                                               3.240972s        1.000000x
std::unordered_map insert                                     2.631293s        1.231703x
fea::unsigned_map insert                                      0.310725s        10.430348x

###########################################
Erase 2500000 (all) small objects at random
###########################################
std::map erase                                                2.061097s        1.000000x
std::unordered_map erase                                      0.599892s        3.435780x
fea::unsigned_map erase                                       0.185926s        11.085584x

#########################################
Erase 2500000 (all) big objects at random
#########################################
std::map erase                                                5.036562s        1.000000x
std::unordered_map erase                                      3.957800s        1.272566x
fea::unsigned_map erase                                       0.618260s        8.146353x

##########################################
Insert 2500000 small objects after reserve
##########################################
std::unordered_map insert                                     0.394593s        1.000000x
std::map insert                                               0.354036s        1.114558x
fea::unsigned_map insert                                      0.025379s        15.548216x

########################################
Insert 2500000 big objects after reserve
########################################
std::map insert                                               3.518457s        1.000000x
std::unordered_map insert                                     2.744652s        1.281932x
fea::unsigned_map insert                                      0.310142s        11.344645x

##############################################
Iterate 2500000 small objects and assign value
##############################################
std::map iterate & assign                                     0.075435s        1.000000x
std::unordered_map iterate & assign                           0.067905s        1.110884x
fea::unsigned_map iterate & assign                            0.038293s        1.969929x

############################################
Iterate 2500000 big objects and assign value
############################################
std::map iterate & assign                                     0.290063s        1.000000x
std::unordered_map iterate & assign                           0.265521s        1.092430x
fea::unsigned_map iterate & assign                            0.240552s        1.205821x



########################################################
Benchmark using linear keys, 2500000 to 0, no duplicates
########################################################
Num unique keys : 2500000

###############################
Copy ctor 2500000 small objects
###############################
std::unordered_map copy ctor                                  1.107623s        1.000000x
std::map copy ctor                                            0.841356s        1.316474x
fea::unsigned_map copy ctor                                   0.032643s        33.931718x

#############################
Copy ctor 2500000 big objects
#############################
std::map copy ctor                                            4.333532s        1.000000x
std::unordered_map copy ctor                                  2.847264s        1.521999x
fea::unsigned_map copy ctor                                   1.186175s        3.653366x

###########################
Clear 2500000 small objects
###########################
std::unordered_map clear                                      0.574819s        1.000000x
std::map clear                                                0.437415s        1.314127x
fea::unsigned_map clear                                       0.000000s        infx

#########################
Clear 2500000 big objects
#########################
std::map clear                                                3.422172s        1.000000x
std::unordered_map clear                                      1.994317s        1.715962x
fea::unsigned_map clear                                       0.000000s        infx

############################
Insert 2500000 small objects
############################
std::unordered_map insert                                     0.381532s        1.000000x
std::map insert                                               0.351536s        1.085328x
fea::unsigned_map insert                                      0.025614s        14.895490x

##########################
Insert 2500000 big objects
##########################
std::map insert                                               3.697916s        1.000000x
std::unordered_map insert                                     2.507451s        1.474771x
fea::unsigned_map insert                                      0.303105s        12.200125x

###########################################
Erase 2500000 (all) small objects at random
###########################################
std::map erase                                                2.166736s        1.000000x
std::unordered_map erase                                      0.651744s        3.324519x
fea::unsigned_map erase                                       0.190653s        11.364799x

#########################################
Erase 2500000 (all) big objects at random
#########################################
std::map erase                                                5.526953s        1.000000x
std::unordered_map erase                                      4.454070s        1.240877x
fea::unsigned_map erase                                       0.620104s        8.912950x

##########################################
Insert 2500000 small objects after reserve
##########################################
std::unordered_map insert                                     0.359057s        1.000000x
std::map insert                                               0.318015s        1.129057x
fea::unsigned_map insert                                      0.023742s        15.123415x

########################################
Insert 2500000 big objects after reserve
########################################
std::map insert                                               3.503023s        1.000000x
std::unordered_map insert                                     2.893732s        1.210555x
fea::unsigned_map insert                                      0.305615s        11.462189x

##############################################
Iterate 2500000 small objects and assign value
##############################################
std::map iterate & assign                                     0.080780s        1.000000x
std::unordered_map iterate & assign                           0.067558s        1.195702x
fea::unsigned_map iterate & assign                            0.037697s        2.142876x

############################################
Iterate 2500000 big objects and assign value
############################################
std::map iterate & assign                                     0.315658s        1.000000x
std::unordered_map iterate & assign                           0.272402s        1.158793x
fea::unsigned_map iterate & assign                            0.250588s        1.259669x



############################################################
Benchmark using 5000000 random uniform distribution keys, with duplicates
############################################################
Num unique keys : 1227019

###############################
Copy ctor 1227019 small objects
###############################
std::unordered_map copy ctor                                  0.459197s        1.000000x
std::map copy ctor                                            0.357469s        1.284580x
fea::unsigned_map copy ctor                                   0.016143s        28.445252x

#############################
Copy ctor 1227019 big objects
#############################
std::map copy ctor                                            2.155539s        1.000000x
std::unordered_map copy ctor                                  1.845563s        1.167957x
fea::unsigned_map copy ctor                                   0.608533s        3.542186x

###########################
Clear 1227019 small objects
###########################
std::unordered_map clear                                      0.319863s        1.000000x
std::map clear                                                0.306773s        1.042670x
fea::unsigned_map clear                                       0.000000s        3198630.000000x

#########################
Clear 1227019 big objects
#########################
std::map clear                                                2.275923s        1.000000x
std::unordered_map clear                                      1.400945s        1.624563x
fea::unsigned_map clear                                       0.000000s        infx

############################
Insert 5000000 small objects
############################
std::map insert                                               3.196918s        1.000000x
std::unordered_map insert                                     0.388278s        8.233589x
fea::unsigned_map insert                                      0.066638s        47.974185x

##########################
Insert 5000000 big objects
##########################
std::map insert                                               5.909745s        1.000000x
std::unordered_map insert                                     3.423378s        1.726290x
fea::unsigned_map insert                                      0.562739s        10.501754x

###########################################
Erase 1227019 (all) small objects at random
###########################################
std::map erase                                                3.022054s        1.000000x
std::unordered_map erase                                      0.345929s        8.736048x
fea::unsigned_map erase                                       0.074532s        40.547339x

#########################################
Erase 1227019 (all) big objects at random
#########################################
std::map erase                                                4.732353s        1.000000x
std::unordered_map erase                                      2.061567s        2.295513x
fea::unsigned_map erase                                       0.295110s        16.035918x

##########################################
Insert 5000000 small objects after reserve
##########################################
std::map insert                                               3.225147s        1.000000x
std::unordered_map insert                                     0.397784s        8.107792x
fea::unsigned_map insert                                      0.078353s        41.161760x

########################################
Insert 5000000 big objects after reserve
########################################
std::map insert                                               6.498736s        1.000000x
std::unordered_map insert                                     3.993628s        1.627276x
fea::unsigned_map insert                                      0.869974s        7.470035x

##############################################
Iterate 1227019 small objects and assign value
##############################################
std::map iterate & assign                                     0.131059s        1.000000x
std::unordered_map iterate & assign                           0.029000s        4.519347x
fea::unsigned_map iterate & assign                            0.018568s        7.058429x

############################################
Iterate 1227019 big objects and assign value
############################################
std::map iterate & assign                                     0.268022s        1.000000x
std::unordered_map iterate & assign                           0.125603s        2.133889x
fea::unsigned_map iterate & assign                            0.117928s        2.272767x



####################################################
Benchmark using 5000000 rand() keys, many duplicates
####################################################
Num unique keys : 32768

#############################
Copy ctor 32768 small objects
#############################
std::map copy ctor                                            0.003609s        1.000000x
std::unordered_map copy ctor                                  0.003542s        1.019116x
fea::unsigned_map copy ctor                                   0.000652s        5.539134x

###########################
Copy ctor 32768 big objects
###########################
fea::unsigned_map copy ctor                                   0.017135s        1.000000x
std::map copy ctor                                            0.012947s        1.323409x
std::unordered_map copy ctor                                  0.011525s        1.486690x

#########################
Clear 32768 small objects
#########################
std::map clear                                                0.001985s        1.000000x
std::unordered_map clear                                      0.001071s        1.852782x
fea::unsigned_map clear                                       0.000000s        infx

#######################
Clear 32768 big objects
#######################
std::map clear                                                0.004911s        1.000000x
std::unordered_map clear                                      0.003670s        1.338156x
fea::unsigned_map clear                                       0.000000s        49109.000000x

############################
Insert 5000000 small objects
############################
std::map insert                                               0.673226s        1.000000x
std::unordered_map insert                                     0.069157s        9.734737x
fea::unsigned_map insert                                      0.019661s        34.242578x

##########################
Insert 5000000 big objects
##########################
std::map insert                                               1.550019s        1.000000x
std::unordered_map insert                                     0.484840s        3.196967x
fea::unsigned_map insert                                      0.262069s        5.914542x

#########################################
Erase 32768 (all) small objects at random
#########################################
std::map erase                                                0.042929s        1.000000x
std::unordered_map erase                                      0.028990s        1.480840x
fea::unsigned_map erase                                       0.008797s        4.879749x

#######################################
Erase 32768 (all) big objects at random
#######################################
std::map erase                                                0.052702s        1.000000x
std::unordered_map erase                                      0.031542s        1.670868x
fea::unsigned_map erase                                       0.011977s        4.400284x

##########################################
Insert 5000000 small objects after reserve
##########################################
std::map insert                                               0.683971s        1.000000x
std::unordered_map insert                                     0.104895s        6.520541x
fea::unsigned_map insert                                      0.019709s        34.703511x

########################################
Insert 5000000 big objects after reserve
########################################
std::map insert                                               1.538086s        1.000000x
std::unordered_map insert                                     0.645134s        2.384132x
fea::unsigned_map insert                                      0.271080s        5.673925x

############################################
Iterate 32768 small objects and assign value
############################################
std::map iterate & assign                                     0.001662s        1.000000x
std::unordered_map iterate & assign                           0.000777s        2.139107x
fea::unsigned_map iterate & assign                            0.000501s        3.320615x

##########################################
Iterate 32768 big objects and assign value
##########################################
std::map iterate & assign                                     0.005385s        1.000000x
std::unordered_map iterate & assign                           0.003320s        1.622108x
fea::unsigned_map iterate & assign                            0.003065s        1.756892x
```


## flat_unsigned_hashmap Benchmarks

```
########################################################
Benchmark using linear keys, 0 to 2500000, no duplicates
########################################################
Num unique keys : 2500000

###############################
Copy ctor 2500000 small objects
###############################
std::unordered_map copy ctor                                  0.509497s        1.000000x
std::map copy ctor                                            0.268229s        1.899482x
fea::flat_unsigned_hashmap copy ctor                          0.053902s        9.452224x

#############################
Copy ctor 2500000 big objects
#############################
std::unordered_map copy ctor                                  2.278400s        1.000000x
std::map copy ctor                                            1.844449s        1.235274x
fea::flat_unsigned_hashmap copy ctor                          1.243165s        1.832741x

###########################
Clear 2500000 small objects
###########################
std::unordered_map clear                                      0.182482s        1.000000x
std::map clear                                                0.115966s        1.573576x
fea::flat_unsigned_hashmap clear                              0.006057s        30.125450x

#########################
Clear 2500000 big objects
#########################
std::unordered_map clear                                      0.618631s        1.000000x
std::map clear                                                0.581321s        1.064180x
fea::flat_unsigned_hashmap clear                              0.005674s        109.027088x

############################
Insert 2500000 small objects
############################
std::unordered_map insert                                     0.365053s        1.000000x
std::map insert                                               0.358080s        1.019475x
fea::flat_unsigned_hashmap insert                             0.124912s        2.922474x

##########################
Insert 2500000 big objects
##########################
std::unordered_map insert                                     1.555538s        1.000000x
std::map insert                                               1.352866s        1.149810x
fea::flat_unsigned_hashmap insert                             0.444110s        3.502597x

###########################################
Erase 2500000 (all) small objects at random
###########################################
std::map erase                                                2.034019s        1.000000x
std::unordered_map erase                                      0.576010s        3.531223x
fea::flat_unsigned_hashmap erase                              0.208066s        9.775830x

#########################################
Erase 2500000 (all) big objects at random
#########################################
std::map erase                                                3.781830s        1.000000x
std::unordered_map erase                                      1.537036s        2.460470x
fea::flat_unsigned_hashmap erase                              0.645497s        5.858787x

##########################################
Insert 2500000 small objects after reserve
##########################################
std::unordered_map insert                                     0.352006s        1.000000x
std::map insert                                               0.313730s        1.122005x
fea::flat_unsigned_hashmap insert                             0.117619s        2.992781x

########################################
Insert 2500000 big objects after reserve
########################################
std::unordered_map insert                                     1.963729s        1.000000x
std::map insert                                               1.257592s        1.561500x
fea::flat_unsigned_hashmap insert                             0.441302s        4.449858x

##############################################
Iterate 2500000 small objects and assign value
##############################################
std::map iterate & assign                                     0.071127s        1.000000x
std::unordered_map iterate & assign                           0.068560s        1.037436x
fea::flat_unsigned_hashmap iterate & assign                   0.039249s        1.812208x

############################################
Iterate 2500000 big objects and assign value
############################################
std::map iterate & assign                                     0.290166s        1.000000x
std::unordered_map iterate & assign                           0.265522s        1.092813x
fea::flat_unsigned_hashmap iterate & assign                   0.234300s        1.238438x



########################################################
Benchmark using linear keys, 2500000 to 0, no duplicates
########################################################
Num unique keys : 2500000

###############################
Copy ctor 2500000 small objects
###############################
std::unordered_map copy ctor                                  0.706340s        1.000000x
std::map copy ctor                                            0.469871s        1.503265x
fea::flat_unsigned_hashmap copy ctor                          0.054182s        13.036512x

#############################
Copy ctor 2500000 big objects
#############################
std::map copy ctor                                            2.638656s        1.000000x
std::unordered_map copy ctor                                  2.006371s        1.315138x
fea::flat_unsigned_hashmap copy ctor                          1.246134s        2.117474x

###########################
Clear 2500000 small objects
###########################
std::unordered_map clear                                      0.288281s        1.000000x
std::map clear                                                0.181371s        1.589450x
fea::flat_unsigned_hashmap clear                              0.006041s        47.719129x

#########################
Clear 2500000 big objects
#########################
std::map clear                                                1.448521s        1.000000x
std::unordered_map clear                                      1.013410s        1.429353x
fea::flat_unsigned_hashmap clear                              0.005665s        255.701036x

############################
Insert 2500000 small objects
############################
std::unordered_map insert                                     0.339890s        1.000000x
std::map insert                                               0.319938s        1.062361x
fea::flat_unsigned_hashmap insert                             0.120231s        2.826964x

##########################
Insert 2500000 big objects
##########################
std::unordered_map insert                                     2.034309s        1.000000x
std::map insert                                               1.287234s        1.580372x
fea::flat_unsigned_hashmap insert                             0.438080s        4.643693x

###########################################
Erase 2500000 (all) small objects at random
###########################################
std::map erase                                                2.108869s        1.000000x
std::unordered_map erase                                      0.636455s        3.313463x
fea::flat_unsigned_hashmap erase                              0.209144s        10.083316x

#########################################
Erase 2500000 (all) big objects at random
#########################################
std::map erase                                                4.347437s        1.000000x
std::unordered_map erase                                      3.049826s        1.425471x
fea::flat_unsigned_hashmap erase                              0.655642s        6.630808x

##########################################
Insert 2500000 small objects after reserve
##########################################
std::unordered_map insert                                     0.374243s        1.000000x
std::map insert                                               0.318082s        1.176563x
fea::flat_unsigned_hashmap insert                             0.123666s        3.026231x

########################################
Insert 2500000 big objects after reserve
########################################
std::unordered_map insert                                     2.526809s        1.000000x
std::map insert                                               2.070252s        1.220532x
fea::flat_unsigned_hashmap insert                             0.443182s        5.701518x

##############################################
Iterate 2500000 small objects and assign value
##############################################
std::map iterate & assign                                     0.078551s        1.000000x
std::unordered_map iterate & assign                           0.068977s        1.138801x
fea::flat_unsigned_hashmap iterate & assign                   0.037631s        2.087387x

############################################
Iterate 2500000 big objects and assign value
############################################
std::map iterate & assign                                     0.295600s        1.000000x
std::unordered_map iterate & assign                           0.279349s        1.058173x
fea::flat_unsigned_hashmap iterate & assign                   0.240102s        1.231146x



############################################################
Benchmark using 5000000 random uniform distribution keys, with duplicates
############################################################
Num unique keys : 1227108

###############################
Copy ctor 1227108 small objects
###############################
std::unordered_map copy ctor                                  0.442366s        1.000000x
std::map copy ctor                                            0.349513s        1.265665x
fea::flat_unsigned_hashmap copy ctor                          0.026470s        16.711798x

#############################
Copy ctor 1227108 big objects
#############################
std::map copy ctor                                            1.563192s        1.000000x
std::unordered_map copy ctor                                  0.916167s        1.706231x
fea::flat_unsigned_hashmap copy ctor                          0.613625s        2.547472x

###########################
Clear 1227108 small objects
###########################
std::map clear                                                0.258953s        1.000000x
std::unordered_map clear                                      0.253569s        1.021231x
fea::flat_unsigned_hashmap clear                              0.003037s        85.257498x

#########################
Clear 1227108 big objects
#########################
std::map clear                                                1.812998s        1.000000x
std::unordered_map clear                                      0.995790s        1.820663x
fea::flat_unsigned_hashmap clear                              0.002563s        707.262893x

############################
Insert 5000000 small objects
############################
std::map insert                                               3.121570s        1.000000x
std::unordered_map insert                                     0.384055s        8.127932x
fea::flat_unsigned_hashmap insert                             0.243779s        12.804909x

##########################
Insert 5000000 big objects
##########################
std::map insert                                               5.610653s        1.000000x
std::unordered_map insert                                     2.448988s        2.291008x
fea::flat_unsigned_hashmap insert                             0.601690s        9.324822x

###########################################
Erase 1227108 (all) small objects at random
###########################################
std::map erase                                                3.006474s        1.000000x
std::unordered_map erase                                      0.354466s        8.481700x
fea::flat_unsigned_hashmap erase                              0.181084s        16.602678x

#########################################
Erase 1227108 (all) big objects at random
#########################################
std::map erase                                                4.457639s        1.000000x
std::unordered_map erase                                      1.862029s        2.393969x
fea::flat_unsigned_hashmap erase                              0.389508s        11.444273x

##########################################
Insert 5000000 small objects after reserve
##########################################
std::map insert                                               3.326453s        1.000000x
std::unordered_map insert                                     0.455664s        7.300227x
fea::flat_unsigned_hashmap insert                             0.264610s        12.571148x

########################################
Insert 5000000 big objects after reserve
########################################
std::map insert                                               6.126258s        1.000000x
std::unordered_map insert                                     2.625268s        2.333575x
fea::flat_unsigned_hashmap insert                             0.968102s        6.328111x

##############################################
Iterate 1227108 small objects and assign value
##############################################
std::map iterate & assign                                     0.128183s        1.000000x
std::unordered_map iterate & assign                           0.028742s        4.459832x
fea::flat_unsigned_hashmap iterate & assign                   0.018366s        6.979386x

############################################
Iterate 1227108 big objects and assign value
############################################
std::map iterate & assign                                     0.266281s        1.000000x
std::unordered_map iterate & assign                           0.127836s        2.082981x
fea::flat_unsigned_hashmap iterate & assign                   0.115270s        2.310052x



####################################################
Benchmark using 5000000 rand() keys, many duplicates
####################################################
Num unique keys : 32768

#############################
Copy ctor 32768 small objects
#############################
std::unordered_map copy ctor                                  0.003367s        1.000000x
std::map copy ctor                                            0.003234s        1.040904x
fea::flat_unsigned_hashmap copy ctor                          0.000690s        4.880690x

###########################
Copy ctor 32768 big objects
###########################
fea::flat_unsigned_hashmap copy ctor                          0.017153s        1.000000x
std::map copy ctor                                            0.012825s        1.337445x
std::unordered_map copy ctor                                  0.011261s        1.523146x

#########################
Clear 32768 small objects
#########################
std::map clear                                                0.002178s        1.000000x
std::unordered_map clear                                      0.001078s        2.020967x
fea::flat_unsigned_hashmap clear                              0.000104s        20.986513x

#######################
Clear 32768 big objects
#######################
std::map clear                                                0.004677s        1.000000x
std::unordered_map clear                                      0.003574s        1.308359x
fea::flat_unsigned_hashmap clear                              0.000106s        44.202268x

#############################
Insert 20000000 small objects
#############################
std::map insert                                               2.765860s        1.000000x
std::unordered_map insert                                     0.265089s        10.433702x
fea::flat_unsigned_hashmap insert                             0.249528s        11.084366x

###########################
Insert 20000000 big objects
###########################
std::map insert                                               6.262053s        1.000000x
std::unordered_map insert                                     2.070184s        3.024877x
fea::flat_unsigned_hashmap insert                             0.653995s        9.575076x

#########################################
Erase 32768 (all) small objects at random
#########################################
fea::flat_unsigned_hashmap erase                              0.151920s        1.000000x
std::unordered_map erase                                      0.112697s        1.348042x
std::map erase                                                0.092785s        1.637332x

#######################################
Erase 32768 (all) big objects at random
#######################################
fea::flat_unsigned_hashmap erase                              0.156057s        1.000000x
std::unordered_map erase                                      0.113882s        1.370347x
std::map erase                                                0.102572s        1.521440x

###########################################
Insert 20000000 small objects after reserve
###########################################
std::map insert                                               2.733880s        1.000000x
std::unordered_map insert                                     0.452906s        6.036315x
fea::flat_unsigned_hashmap insert                             0.242918s        11.254338x

#########################################
Insert 20000000 big objects after reserve
#########################################
std::map insert                                               6.304130s        1.000000x
std::unordered_map insert                                     3.077564s        2.048416x
fea::flat_unsigned_hashmap insert                             0.670298s        9.404971x

############################################
Iterate 32768 small objects and assign value
############################################
std::map iterate & assign                                     0.001559s        1.000000x
std::unordered_map iterate & assign                           0.000768s        2.030070x
fea::flat_unsigned_hashmap iterate & assign                   0.000493s        3.161362x

##########################################
Iterate 32768 big objects and assign value
##########################################
std::map iterate & assign                                     0.005469s        1.000000x
std::unordered_map iterate & assign                           0.003355s        1.629948x
fea::flat_unsigned_hashmap iterate & assign                   0.003018s        1.811941x
```
