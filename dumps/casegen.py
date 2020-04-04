st = "N_LT,N_LE,N_EQ,N_NE,N_GE,N_GT,N_AND2,N_OR2"

nodes = st.split(",")

txt = """
case %s:"""

for node in nodes:
    print(txt % (node), end = "")
    