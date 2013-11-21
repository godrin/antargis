

def translate(type,value)
  tx={
    :hunger=>["full",0.2,"satiated",0.5,"hungry",0.7,"very hungry",0.9,"starving"]
  }

  a=tx[type]
  i=0
  while i+1<a.length && a[i+1]<value
    i+=2
  end
  a[i]

end


