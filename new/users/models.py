from django.db import models

# Create your models here.

class Register(models.Model):
    username = models.CharField('username', max_length=128, unique = True)
    first_name = models.CharField('first_name', max_length=128)
    middle_name = models.CharField('middle_name', max_length=128, blank=True)
    last_name = models.CharField('last_tname', max_length=128)
    occupation = models.CharField('occupation', max_length=128, blank=True)
    phone_number = models.CharField('telephone', max_length=50,blank=True)
    password = models.CharField('password',max_length=20)
    mail_address = models.CharField('mail_Address', max_length=150)
    email = models.CharField('email', max_length=150)