import json

UNLIMITED=-3
E_WE=-1
E_SN=-2

header = {
    "dimensions": {
        "e_we": E_WE,
        "e_sn": E_SN,
    },    
    "variables": {},
    "global_attributes": []
}

SECS = {
    "dimensions": "dimensions",
    "variables": "variables",
    "// global attributes": "global_attributes"
}

current_sec = None
with open("header.txt", "r") as file:
    for line in file:
        line = line.strip()
        update_sec = False
        for sec_key in SECS:
            if line.startswith(sec_key):
                current_sec = SECS[sec_key]
                update_sec = True
        if current_sec is None:
            continue
        if update_sec:
            continue
        
        line = line.split("//")[0].strip()[:-1].strip()
        if not line:
            continue
        if current_sec == "dimensions":
            # now line is like: "key = value"
            # parse key and value
            key, value = line.split("=")
            key = key.strip()
            value = value.strip()
            if value == "UNLIMITED":
                value = UNLIMITED

            header["dimensions"][key] = int(value)

            continue
        if current_sec == "variables":
            if "=" not in line:
                variable = {
                    "attributes": [],
                    "num_attributes": 0,
                    "dimensions": [],
                    "num_dimensions": 0,
                    "is_partition": False,
                }
                # float C3F(Time, bottom_top_stag)
                variable["type"] = line.split()[0]
                variable["name"] = line.split()[1].split("(")[0]
                variable["dimensions"] = line.split("(")[1].split(")")[0].split(",")
                variable["dimensions"] = [d.strip() for d in variable["dimensions"]]
                variable["num_dimensions"] = len(variable["dimensions"])
                for i in range(variable["num_dimensions"]):
                    if variable["dimensions"][i].startswith("west_east"):
                        variable["dimensions"][i] = "e_we"
                        variable["is_partition"] = True
                    if variable["dimensions"][i].startswith("south_north"):
                        variable["dimensions"][i] = "e_sn"
                        variable["is_partition"] = True
                    dim_name = variable["dimensions"][i]                    
                if header["dimensions"][variable["dimensions"][0]] == UNLIMITED:
                    variable["dimensions"] = variable["dimensions"][1:]
                # print(variable["dimensions"])
                variable["num_dimensions"] = len(variable["dimensions"])
                header["variables"][variable["name"]] = variable
                continue
            # line = XLAT:FieldType = 104
            key, value = [x.strip() for x in line.split("=", 1)]
            var_name, attr_name = key.split(":")
            if value.endswith("f"):
                value = value[:-1] + "00"
            value = eval(value)
            header["variables"][var_name]["attributes"].append({
                "attr_name": attr_name,
                "value": value,
                "type": type(value).__name__
            })
            header["variables"][var_name]["num_attributes"] += 1
            continue
        
        if current_sec == "global_attributes":
            key, value = [x.strip() for x in line.split("=", 1)]
            var_name, attr_name = key.split(":")
            if value.endswith("f"):
                value = value[:-1] + "00"
            value = eval(value)
            header["global_attributes"].append({
                "attr_name": attr_name,
                "value": value,
                "type": type(value).__name__
            })
            continue

output = {"variables":[], "global_attributes": header["global_attributes"], "dimensions": []}
for dim_name in header["dimensions"]:
    output["dimensions"].append({"name": dim_name, "value": header["dimensions"][dim_name]} )


for var_name in header["variables"]:
    output["variables"].append(header["variables"][var_name])
with open("header.json", "w") as file:
    json.dump(output, file, indent=2)
    