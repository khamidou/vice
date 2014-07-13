command_mode = true
display("--COMMAND--")

command_buffer = ""

function keyboard_event_handler (keyname)
    io.write(keyname, "\n")
    if command_mode == true then
        if keyname == "Return" then
            io.write("Full command: ", command_buffer, "\n")
            command_buffer = ""
        else
            command_buffer = command_buffer .. keyname
        end
    end
end
