def write_stl(vertices, filename):
    with open(filename,'rb') as f:
        f.write("This is a binary .STL file exported from antimony " +
                "(github.com/mkeeter/antimony).")
        f.write(struct.pack("<I", len(vertices)/3))
        for tri in np.array_split(vertices, len(vertices)/9):
            f.write(struct.pack("<fff", 0, 0, 0))   # normal
            f.write(struct.pack("<fffffffff", *tri)) # triangle
            f.write(struct.pack("<H", 0))   # attributes

