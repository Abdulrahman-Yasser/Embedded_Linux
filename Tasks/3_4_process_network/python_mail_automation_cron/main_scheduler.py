import mail_wrapper


subject = "Trying the automation"


file = open("/home/abdu/Study/Embedded_Linux/git_ignoring/private_files/mail_data.txt", "r")

mail = file.read()
passwd = file.read()
print(mail)
print(passwd)
file.close()

body = """Dear abdu, 
        I hope you are doing well!
        I was talkling about going into a depate with zobry.
        Lastly, zobry manga."""

my_mail = mail_wrapper.my_gmail(mail, app_password)
my_mail.send_msg_no_resume(subject, body, "abdu.ya74@gmail.com")
my_mail.quit()