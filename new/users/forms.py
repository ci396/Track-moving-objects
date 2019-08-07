from django import forms



class UserRegister(forms.Form):
    username = forms.CharField(label = 'username', max_length = 128)
    first_name = forms.CharField(label = 'first_name', max_length=128)
    middle_name = forms.CharField(max_length=128)
    last_name = forms.CharField(max_length=128)
    occupation = forms.CharField(max_length=128)
    password1 = forms.CharField(label='password',widget=forms.PasswordInput())
    password2 = forms.CharField(label='confirm passowrd',widget=forms.PasswordInput())
    email = forms.EmailField(max_length=150)
    phone_number = forms.CharField(max_length=50)
    mail_address = forms.CharField(max_length=150)

class UserLogin(forms.Form):
    username = forms.CharField(label='username', max_length=100)
    password = forms.CharField(label='password', widget=forms.PasswordInput())

class ChangepwdForm(forms.Form):
    old_password = forms.CharField(label='Old password',widget=forms.PasswordInput)
    new_password = forms.CharField(label='New password',widget=forms.PasswordInput)
    new_password2= forms.CharField(label='Confirm password',widget=forms.PasswordInput)


