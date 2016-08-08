float XformTable[] = {
    0.000000,    0.000000,    0.851600,    0.000000,    1.000000,    0.851600,    1.956400,
    1.000000,    2.000000,    1.956400,    2.728600,    2.000000,    3.000000,    2.728600,
    3.327400,    3.000000,    4.000000,    3.327400,    3.983900,    3.000000,    5.000000,
    3.983900,    4.697900,    4.000000,    6.000000,    4.697900,    5.472800,    5.000000,
    7.000000,    5.472800,    6.314200,    5.000000,    8.000000,    6.314200,    7.213200,
    6.000000,    9.000000,    7.213200,    8.169800,    7.000000,    10.000000,   8.169800,
    9.184000,    8.000000,    11.000000,   9.184000,    10.255800,   9.000000,    12.000000,
    10.255800,   11.385200,   10.000000,   13.000000,   11.385200,   12.578600,   11.000000,
    14.000000,   12.578600,   13.835400,   13.000000,   15.000000,   13.835400,   15.149800,
    14.000000,   16.000000,   15.149800,   16.521799,   15.000000,   17.000000,   16.521799,
    17.951401,   17.000000,   18.000000,   17.951401,   19.438499,   18.000000,   19.000000,
    19.438499,   20.983299,   20.000000,   20.000000,   20.983299,   22.595301,   21.000000,
    21.000000,   22.595301,   24.267300,   23.000000,   22.000000,   24.267300,   25.997101,
    25.000000,   23.000000,   25.997101,   27.784401,   26.000000,   24.000000,   27.784401,
    29.629400,   28.000000,   25.000000,   29.629400,   31.531900,   30.000000,   26.000000,
    31.531900,   33.492599,   32.000000,   27.000000,   33.492599,   35.522499,   34.000000,
    28.000000,   35.522499,   37.610001,   36.000000,   29.000000,   37.610001,   39.755100,
    38.000000,   30.000000,   39.755100,   41.957802,   40.000000,   31.000000,   41.957802,
    44.218102,   43.000000,   32.000000,   44.218102,   46.535999,   45.000000,   33.000000,
    46.535999,   48.915298,   47.000000,   34.000000,   48.915298,   51.360600,   50.000000,
    35.000000,   51.360600,   53.863400,   52.000000,   36.000000,   53.863400,   56.423801,
    55.000000,   37.000000,   56.423801,   59.041901,   57.000000,   38.000000,   59.041901,
    61.717602,   60.000000,   39.000000,   61.717602,   64.450897,   63.000000,   40.000000,
    64.450897,   67.248703,   65.000000,   41.000000,   67.248703,   70.109299,   68.000000,
    42.000000,   70.109299,   73.027496,   71.000000,   43.000000,   73.027496,   76.003304,
    74.000000,   44.000000,   76.003304,   79.036797,   77.000000,   45.000000,   79.036797,
    82.127701,   80.000000,   46.000000,   82.127701,   85.276299,   83.000000,   47.000000,
    85.276299,   88.492798,   86.000000,   48.000000,   88.492798,   91.768799,   90.000000,
    49.000000,   91.768799,   95.102303,   93.000000,   50.000000,   95.102303,   98.493500,
    96.000000,   51.000000,   98.493500,   101.942299,  100.000000,  52.000000,   101.942299,
    105.448601,  103.000000,  53.000000,   105.448601,  109.013901,  107.000000,  54.000000,
    109.013901,  112.647697,  110.000000,  55.000000,   112.647697,  116.338997,  114.000000,
    56.000000,   116.338997,  120.087898,  118.000000,  57.000000,   120.087898,  123.894402,
    121.000000,  58.000000,   123.894402,  127.758598,  125.000000,  59.000000,   127.758598,
    131.680298,  129.000000,  60.000000,   131.680298,  135.664093,  133.000000,  61.000000,
    135.664093,  139.713196,  137.000000,  62.000000,   139.713196,  143.819901,  141.000000,
    63.000000,   143.819901,  147.984100,  145.000000,  64.000000,   147.984100,  152.205994,
    150.000000,  65.000000,   152.205994,  156.485596,  154.000000,  66.000000,   156.485596,
    160.822601,  158.000000,  67.000000,   160.822601,  165.225006,  163.000000,  68.000000,
    165.225006,  169.689407,  167.000000,  69.000000,   169.689407,  174.211502,  171.000000,
    70.000000,   174.211502,  178.791107,  176.000000,  71.000000,   178.791107,  183.428406,
    181.000000,  72.000000,   183.428406,  188.123306,  185.000000,  73.000000,   188.123306,
    192.875702,  190.000000,  74.000000,   192.875702,  197.696594,  195.000000,  75.000000,
    197.696594,  202.576401,  200.000000,  76.000000,   202.576401,  207.513794,  205.000000,
    77.000000,   207.513794,  212.508804,  210.000000,  78.000000,   212.508804,  217.561401,
    215.000000,  79.000000,   217.561401,  222.671707,  220.000000,  80.000000,   222.671707,
    227.841400,  225.000000,  81.000000,   227.841400,  233.078995,  230.000000,  82.000000,
    233.078995,  238.374100,  235.000000,  83.000000,   238.374100,  243.726898,  241.000000,
    84.000000,   243.726898,  249.137299,  246.000000,  85.000000,   249.137299,  254.605194,
    251.000000,  86.000000,   254.605194,  260.130798,  257.000000,  87.000000,   260.130798,
    265.719086,  262.000000,  88.000000,   265.719086,  271.372009,  268.000000,  89.000000,
    271.372009,  277.082611,  274.000000,  90.000000,   277.082611,  282.850708,  279.000000,
    91.000000,   282.850708,  288.676392,  285.000000,  92.000000,   288.676392,  294.559692,
    291.000000,  93.000000,   294.559692,  300.500702,  297.000000,  94.000000,   300.500702,
    306.507507,  303.000000,  95.000000,   306.507507,  312.575806,  309.000000,  96.000000,
    312.575806,  318.701691,  315.000000,  97.000000,   318.701691,  324.885193,  321.000000,
    98.000000,   324.885193,  331.126312,  328.000000,  99.000000,   331.126312,  337.424988,
    334.000000,  100.000000,  337.424988,  343.781189,  340.000000,  101.000000,  343.781189,
    350.206604,  346.000000,  102.000000,  350.206604,  356.690308,  353.000000,  103.000000,
    356.690308,  363.231598,  359.000000,  104.000000,  363.231598,  369.830414,  366.000000,
    105.000000,  369.830414,  376.486908,  373.000000,  106.000000,  376.486908,  383.200897,
    379.000000,  107.000000,  383.200897,  389.975098,  386.000000,  108.000000,  389.975098,
    396.816589,  393.000000,  109.000000,  396.816589,  403.715515,  400.000000,  110.000000,
    403.715515,  410.672089,  407.000000,  111.000000,  410.672089,  417.686401,  414.000000,
    112.000000,  417.686401,  424.758209,  421.000000,  113.000000,  424.758209,  431.887604,
    428.000000,  114.000000,  431.887604,  439.080292,  435.000000,  115.000000,  439.080292,
    446.337097,  442.000000,  116.000000,  446.337097,  453.651489,  449.000000,  117.000000,
    453.651489,  461.023407,  457.000000,  118.000000,  461.023407,  468.453003,  464.000000,
    119.000000,  468.453003,  475.940186,  472.000000,  120.000000,  475.940186,  483.484894,
    479.000000,  121.000000,  483.484894,  491.096313,  487.000000,  122.000000,  491.096313,
    498.768402,  494.000000,  123.000000,  498.768402,  506.498199,  502.000000,  124.000000,
    506.498199,  514.285522,  510.000000,  125.000000,  514.285522,  522.130371,  518.000000,
    126.000000,  522.130371,  530.032898,  526.000000,  127.000000,  530.032898,  537.993103,
    534.000000,  128.000000,  537.993103,  546.023010,  542.000000,  129.000000,  546.023010,
    554.110474,  550.000000,  130.000000,  554.110474,  562.255493,  558.000000,  131.000000,
    562.255493,  570.458191,  566.000000,  132.000000,  570.458191,  578.718506,  574.000000,
    133.000000,  578.718506,  587.036377,  582.000000,  134.000000,  587.036377,  595.415100,
    591.000000,  135.000000,  595.415100,  603.860291,  599.000000,  136.000000,  603.860291,
    612.363220,  608.000000,  137.000000,  612.363220,  620.923584,  616.000000,  138.000000,
    620.923584,  629.541687,  625.000000,  139.000000,  629.541687,  638.217285,  633.000000,
    140.000000,  638.217285,  646.950623,  642.000000,  141.000000,  646.950623,  655.747803,
    651.000000,  142.000000,  655.747803,  664.608521,  660.000000,  143.000000,  664.608521,
    673.526672,  669.000000,  144.000000,  673.526672,  682.502502,  678.000000,  145.000000,
    682.502502,  691.535889,  687.000000,  146.000000,  691.535889,  700.626892,  696.000000,
    147.000000,  700.626892,  709.775513,  705.000000,  148.000000,  709.775513,  718.991272,
    714.000000,  149.000000,  718.991272,  728.267273,  723.000000,  150.000000,  728.267273,
    737.600891,  732.000000,  151.000000,  737.600891,  746.992004,  742.000000,  152.000000,
    746.992004,  756.440674,  751.000000,  153.000000,  756.440674,  765.947205,  761.000000,
    154.000000,  765.947205,  775.511780,  770.000000,  155.000000,  775.511780,  785.145508,
    780.000000,  156.000000,  785.145508,  794.836792,  789.000000,  157.000000,  794.836792,
    804.585693,  799.000000,  158.000000,  804.585693,  814.392212,  809.000000,  159.000000,
    814.392212,  824.256409,  819.000000,  160.000000,  824.256409,  834.178101,  829.000000,
    161.000000,  834.178101,  844.161316,  839.000000,  162.000000,  844.161316,  854.210388,
    849.000000,  163.000000,  854.210388,  864.317078,  859.000000,  164.000000,  864.317078,
    874.481384,  869.000000,  165.000000,  874.481384,  884.703186,  879.000000,  166.000000,
    884.703186,  894.982727,  889.000000,  167.000000,  894.982727,  905.319824,  900.000000,
    168.000000,  905.319824,  915.721619,  910.000000,  169.000000,  915.721619,  926.185974,
    920.000000,  170.000000,  926.185974,  936.708130,  931.000000,  171.000000,  936.708130,
    947.287720,  941.000000,  172.000000,  947.287720,  957.924988,  952.000000,  173.000000,
    957.924988,  968.619812,  963.000000,  174.000000,  968.619812,  979.372192,  973.000000,
    175.000000,  979.372192,  990.192505,  984.000000,  176.000000,  990.192505,  1001.072388,
    995.000000,  177.000000,  1001.072388, 1012.009827, 1006.000000, 178.000000,  1012.009827,
    1023.004822, 1017.000000, 179.000000,  1023.004822, 1034.057251, 1028.000000, 180.000000,
    1034.057251, 1045.167603, 1039.000000, 181.000000,  1045.167603, 1056.336670, 1050.000000,
    182.000000,  1056.336670, 1067.574219, 1061.000000, 183.000000,  1067.574219, 1078.869385,
    1073.000000, 184.000000,  1078.869385, 1090.222168, 1084.000000, 185.000000,  1090.222168,
    1101.632446, 1095.000000, 186.000000,  1101.632446, 1113.100464, 1107.000000, 187.000000,
    1113.100464, 1124.626099, 1118.000000, 188.000000,  1124.626099, 1136.213745, 1130.000000,
    189.000000,  1136.213745, 1147.866577, 1142.000000, 190.000000,  1147.866577, 1159.577148,
    1153.000000, 191.000000,  1159.577148, 1171.345459, 1165.000000, 192.000000,  1171.345459,
    1183.171143, 1177.000000, 193.000000,  1183.171143, 1195.054443, 1189.000000, 194.000000,
    1195.054443, 1206.995239, 1201.000000, 195.000000,  1206.995239, 1219.001465, 1212.000000,
    196.000000,  1219.001465, 1231.069824, 1225.000000, 197.000000,  1231.069824, 1243.195679,
    1237.000000, 198.000000,  1243.195679, 1255.379150, 1249.000000, 199.000000,  1255.379150,
    1267.620239, 1261.000000, 200.000000,  1267.620239, 1279.918945, 1273.000000, 201.000000,
    1279.918945, 1292.275269, 1286.000000, 202.000000,  1292.275269, 1304.699951, 1298.000000,
    203.000000,  1304.699951, 1317.183716, 1310.000000, 204.000000,  1317.183716, 1329.724854,
    1323.000000, 205.000000,  1329.724854, 1342.323853, 1336.000000, 206.000000,  1342.323853,
    1354.980225, 1348.000000, 207.000000,  1354.980225, 1367.694336, 1361.000000, 208.000000,
    1367.694336, 1380.467773, 1374.000000, 209.000000,  1380.467773, 1393.309326, 1386.000000,
    210.000000,  1393.309326, 1406.208252, 1399.000000, 211.000000,  1406.208252, 1419.164917,
    1412.000000, 212.000000,  1419.164917, 1432.179077, 1425.000000, 213.000000,  1432.179077,
    1445.250854, 1438.000000, 214.000000,  1445.250854, 1458.380249, 1451.000000, 215.000000,
    1458.380249, 1471.572510, 1464.000000, 216.000000,  1471.572510, 1484.829224, 1478.000000,
    217.000000,  1484.829224, 1498.143555, 1491.000000, 218.000000,  1498.143555, 1511.515625,
    1504.000000, 219.000000,  1511.515625, 1524.945068, 1518.000000, 220.000000,  1524.945068,
    1538.432251, 1531.000000, 221.000000,  1538.432251, 1551.977051, 1545.000000, 222.000000,
    1551.977051, 1565.587646, 1558.000000, 223.000000,  1565.587646, 1579.259766, 1572.000000,
    224.000000,  1579.259766, 1592.989624, 1586.000000, 225.000000,  1592.989624, 1606.776855,
    1599.000000, 226.000000,  1606.776855, 1620.621826, 1613.000000, 227.000000,  1620.621826,
    1634.524414, 1627.000000, 228.000000,  1634.524414, 1648.484497, 1641.000000, 229.000000,
    1648.484497, 1662.513672, 1655.000000, 230.000000,  1662.513672, 1676.601318, 1669.000000,
    231.000000,  1676.601318, 1690.746460, 1683.000000, 232.000000,  1690.746460, 1704.948975,
    1697.000000, 233.000000,  1704.948975, 1719.209351, 1712.000000, 234.000000,  1719.209351,
    1733.527222, 1726.000000, 235.000000,  1733.527222, 1747.905273, 1740.000000, 236.000000,
    1747.905273, 1762.350464, 1755.000000, 237.000000,  1762.350464, 1776.853394, 1769.000000,
    238.000000,  1776.853394, 1791.413818, 1784.000000, 239.000000,  1791.413818, 1806.031860,
    1798.000000, 240.000000,  1806.031860, 1820.707520, 1813.000000, 241.000000,  1820.707520,
    1835.440796, 1828.000000, 242.000000,  1835.440796, 1850.237427, 1842.000000, 243.000000,
    1850.237427, 1865.098022, 1857.000000, 244.000000,  1865.098022, 1880.016235, 1872.000000,
    245.000000,  1880.016235, 1894.991943, 1887.000000, 246.000000,  1894.991943, 1910.025391,
    1902.000000, 247.000000,  1910.025391, 1925.116455, 1917.000000, 248.000000,  1925.116455,
    1940.265015, 1932.000000, 249.000000,  1940.265015, 1955.480225, 1947.000000, 250.000000,
    1955.480225, 1970.756104, 1963.000000, 251.000000,  1970.756104, 1986.089722, 1978.000000,
    252.000000,  1986.089722, 2001.480957, 1993.000000, 253.000000,  2001.480957, 2016.929565,
    2009.000000, 254.000000,  2016.929565, 2032.436035, 2024.000000, 255.000000,  2032.436035,
    2048.000000, 2040.000000};
