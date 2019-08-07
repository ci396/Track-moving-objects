from django.shortcuts import render,redirect
import hashlib
from .models import Register
from django.template import RequestContext
from .forms import UserLogin,UserRegister,ChangepwdForm
from django.contrib.auth.models import User
from django.http import HttpResponse
from django.http import HttpResponseRedirect
from django.contrib.auth import authenticate,login as auth_login,logout as auth_logout

# Create your views here.
# def take_md5(content):
#     hash = hashlib.md5()    #创建hash加密实例
#     hash.update(content)    #hash加密
#     result = hash.hexdigest()  #得到加密结果
#     return result

#注册
def register(request):

    if request.method == 'POST':
        form = UserRegister(request.POST)
        if form.is_valid():
            username = form.cleaned_data['username']
            first_name = form.cleaned_data['first_name']
            middle_name = form.cleaned_data['middle_name']
            last_name = form.cleaned_data['last_name']
            namefilter = Register.objects.filter(username = username)
            email = form.cleaned_data['email']
            phone_number = form.cleaned_data['phone_number']
            occupation = form.cleaned_data['occupation']
            mail_address = form.cleaned_data['mail_address']
            if len(namefilter) > 0:
                return render(request,'register.html',{'error':'User is already existed','form':form})
            else:
                password1 = form.cleaned_data['password1']
                password2 = form.cleaned_data['password2']
                if password1 != password2:
                    return render(request,'register.html',{'form':form,'error':'Passwords should be same！'})
                else:
                    password = password1


                    user = Register.objects.create(username=username,first_name=first_name,middle_name=middle_name,
                                                   last_name=last_name,occupation=occupation,password=password,email=email,
                                                   phone_number=phone_number,mail_address=mail_address)
                    user.save()
                    return render(request,'success.html',{'username':username,'operation':'Register'})
    else:
        form = UserRegister()
        return render(request,'register.html',{'form':form})



def login(request):

    if request.method == 'POST':
        form = UserLogin(request.POST)
        if form.is_valid(): #
            username = form.cleaned_data['username']
            password = form.cleaned_data['password']
            password = password
            namefilter = Register.objects.filter(username=username,password=password)
            if len(namefilter) > 0:
                currentuser = Register.objects.get(username__exact=username)
                first_name = currentuser.first_name
                middle_name = currentuser.middle_name
                last_name = currentuser.last_name
                occupation = currentuser.occupation
                email = currentuser.email
                mail_address = currentuser.mail_address
                phone_number = currentuser.phone_number
                form.cleaned_data['password']
                return render(request,'success.html',{'username':username,'operation':'Log In','first_name':first_name,
                                                      'middle_name': middle_name, 'last_name': last_name, 'occupation':occupation,
                                                      'email':email,'mailing_address':mail_address,'phone_number':phone_number})
            else:
                return render(request,'login.html',{'error':'Invalid username or password！','form':form})
    else:
        form = UserLogin()
        return render(request,'login.html',{'form':form})

def logout(request):
    auth_logout(request)
    return HttpResponseRedirect('/login/')

def changepassword(request,username):
    error = []
    if request.method == 'POST':
        form = ChangepwdForm(request.POST)
        if form.is_valid():
            data = form.cleaned_data
            #user = authenticate(username=username,password=data['old_password'],UserProfile=Register)
            namefilter = Register.objects.filter(username=username, password=data['old_password'])
            if len(namefilter) > 0:
                if data['new_password']==data['new_password2']:
                    newuser = Register.objects.get(username__exact=username)
                    newuser.password = data['new_password']
                    newuser.save()
                    return HttpResponseRedirect('/login/')
                else:
                    error.append('Please input the same password')
            else:
                error.append('Please correct the old password')
        else:
            error.append('Please input the required domain')
    else:
        form = ChangepwdForm()
    return render(request,'changepassword.html',{'form':form,'error':error})

