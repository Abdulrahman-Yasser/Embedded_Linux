import smtplib 
import csv_data_wrapper
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
import os
import re

class my_gmail:
    def __init__(self, email, password):
        if not (isinstance(email, str) & isinstance(password, str)):
            return
        if 
        self.email = email
        self.password = password
        # SMTP Server and port no for GMAIL.com
        gmail_server= "smtp.gmail.com"
        gmail_port= 587


        # Starting connection
        self.my_server = smtplib.SMTP(gmail_server, gmail_port)
        self.my_server.ehlo()
        self.my_server.starttls()
            
        # Login with your email and password
        self.my_server.login(email, password)

    def send_msg_with_resume(self, subject, msg, to_mail):
        message = MIMEMultipart("alternative")
        text_content=  msg
        message.attach(MIMEText(text))
        # Read the file from location
        resume_file = 'Abdulrahman-Yasser-ES-SW.pdf'
        with open(resume_file, 'rb') as f:
            file = MIMEApplication(
                f.read(),
                name=os.path.basename(resume_file)
            )
            # file['Content-Disposition'] = f'attachment; 


            # filename="{os.path.basename(resume_file)}"'
            # message.attach(file)
        self.my_server.sendmail(self.email, to_mail, msg)

    def send_msg_no_resume(self, subject, msg, to_mail):
        msg = "Subject: " + subject+'\n' + msg
        self.my_server.sendmail(self.email, to_mail, msg)
        # Read the file from location
    
    def quit(self):
        self.my_server.quit()

    def __mail_validate(self, to_mail):
        pass
